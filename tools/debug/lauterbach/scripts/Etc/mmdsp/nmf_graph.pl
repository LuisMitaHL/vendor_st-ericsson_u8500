use strict;
use GraphViz;

my %wanted_interfaces=(empty       => { methods => ["emptyThisBuffer"],
										display => "color",
										color   => "green"},
					   fill        => { methods => ["fillThisBuffer"],
										display => "none"},
					   sendcommand => { methods => ["sendCommand"],
										display => "color",
										color   => "red"},
					   process     => { methods => ["processEvent"],
										display => "compo_filled" ,
										fill_color => "slategray2"},
					   effect      => { methods => ["open","reset","processBuffer","newFormat"],
										display => "record"},
					   decoder     => { methods => ["decodeFrame","checkSynchro","open","reset","setOutputBuf","getMaxFrameSize","getOutputGrain","getHeaderType","getSampleBitSize","getBlockSize"],
										display => "record"},
					   encoder     => { methods => ["encodeFrame","open","reset","getMaxFrameSize","getMaxSamples","getSampleBitSize"],
										display => "record"}
	);
my @ignored_components=("hybrid_hsem");
my %components;
my $trace = shift;
my $output_graph = shift || "nmf_graph.png";

die "no input file" unless $trace;

open TRACE, "$trace" or die "Cannot open $trace : $!";


## First step : parse log and keep only interresting components and interfaces
while(my $line=<TRACE>)
{
	if($line =~ /== \[(\w+)\] \[([\w\$]+)\] ==/)
	{
		&process_one_component($1,$2);
	}
}
#&list_components();

## Second Step : find asynchronous bind
&find_asynchronous_interface_and_fill();
#&list_components();

## Third step generate graph
&generate_graph();



###################################
## Subroutines
##################################
sub this{
	my $this = $_[0];
	$this =~ s/0x0+([A-Fa-f0-9])/0x\1/;
	return $this;
}

sub is_ignored_component{
	my $name = $_[0];

	for (my $i=0; $i <= $#ignored_components;$i++)
	{
		return 1 if ($ignored_components[$i] eq $name);
	}

	return 0;
}

sub is_wanted_interface{
	my $methods = $_[0];
	my $methods_list = join(':',@$methods);

	foreach my $itf (sort(keys %wanted_interfaces))
	{
		my $test_list = join(':',@{$wanted_interfaces{$itf}->{methods}});
		return $itf if ($methods_list eq $test_list);
	}

	return "no";
}

sub process_one_component{
	my($this,$name) = @_;

	$name =~ s/\$/_/g;

	return if &is_ignored_component($name);

	$this = &this($this);
	$components{$this}{name}=$name;

	while(my $line=<TRACE>)
	{
		if($line =~ /B::var.watch_"==_\[/)
		{   # begining of next component
			return;
		}
		elsif($line =~ /\s*(.*\\)?(\w+)`? = \(/)
		{
			#new interface
			my $itf_name = $2;
			$itf_name =~ s/${name}_//;

			$line=<TRACE>;
			if($line =~ /^\s*(\(.+\)\s*)?THIS\s+=\s+(\w+),/)
			{
				# simple interface
				my $dest_this = &this($2);
				my @methods;

				while($line=<TRACE>)
				{
					if($line =~ /(\w+)\s+=\s+(\w+)(,|\))/)
					{
						last if($2 eq "0x0");
						push @methods, $1;
						last if($3 eq ")");
					}
					else
					{
						print "Error in process_one_component (simple interface) :\n$line [$.]\n";
					}
				}
				&store_interface($this,$dest_this,\@methods,$itf_name);
			}
			else
			{
				while($line =~ /\[(\w+)\] = \((\(.+\)\s*)?THIS = (\w+), (.*)\)(,|\))$/)
				{
					my $idx=$1;
					my $dest_this=&this($3);
					my $methods_list=$4;
					my @methods;
					if (&is_interface_binded($methods_list))
					{
						$methods_list =~ s/(\([^=]+\)\s*)?(\w+)\s+=\s+(\w+)(,|\)|$)/\2/g;
						@methods = split(/ /,$methods_list);
					}
					$idx=hex($idx) if ($idx=~/0x/i);
					&store_interface($this,$dest_this,\@methods,$itf_name."_$idx");
					last if $line =~ /\)\)\s*$/;
					$line = <TRACE>;
				}
			}
		}
	}
}

sub is_interface_binded {

	my $line = $_[0];
	my $test = $line;

	# consider the interface binded if ALL methods have a valid address
	while ($line =~ /\w+ = (\w+)/)
	{
		return 0 if (hex($1)==0);
		$line = $`.$'; #'
	}

	return 1;
}

sub store_interface {
	my($this,$dest_this,$methods,$itf) = @_;
	my $nb_methods = @$methods;
	my $itf_type;

	return unless $nb_methods;

	$itf_type = &is_wanted_interface($methods);
	return if ($itf_type eq "no");

	$components{$this}{output_connection}{$itf} = { destination => $dest_this ,
													type        => $itf_type};

	$components{$dest_this}{input_connection}{$itf}  = { source => $this ,
														 type   => $itf_type};

	if($wanted_interfaces{$itf_type}->{display} eq "record")
	{
		$components{$dest_this}{ignore}="yes";
	}
#	print "$components{$this}{name} [$this] $itf => [$dest_this] \n";
}




sub find_asynchronous_interface_and_fill
{
	my @delete_list;
	foreach my $compo (sort(keys %components))
	{
		if($components{$compo}{output_connection})
		{
			foreach my $itf (sort(keys %{$components{$compo}{output_connection}}))
			{
				my $dest_this = $components{$compo}{output_connection}{$itf}->{destination};
				if($components{$dest_this}{name} =~ /^_ev_/)
				{
					$components{$compo}{output_connection}{$itf}->{asynchronous} = "yes";

					if(defined($components{$dest_this}{output_connection}{target}))
					{
						my $target = $components{$dest_this}{output_connection}{target}->{destination};
						$components{$compo}{output_connection}{$itf}->{destination} = $target;
						$components{$target}{input_connection}{target}->{source}    = $compo;
						push @delete_list, $dest_this; #cannot delete here
					}
					else
					{
						die "error in asynchronous bind"
					}
				}
			}
		}

		if($components{$compo}{input_connection})
		{
			my @fill;
			foreach my $itf (sort(keys %{$components{$compo}{input_connection}}))
			{
				if ($components{$compo}{input_connection}{$itf}->{type} eq "fill")
				{
					push @fill, $components{$compo}{input_connection}{$itf}->{source};
				}
			}
			while (my $source = shift(@fill))
			{
				if(! &is_empty_connection($compo,$source) &&
				   ($components{$source}{name} !~ /^_(st|sk|ev)_/))
				{
					$components{$compo}{output_connection}{empty} = {destination => $source,
																	 type        => "empty"};
				}
			}
		}
	}

	while (my $elt = shift(@delete_list))
	{
		delete $components{$elt};
	}
}


sub is_empty_connection
{
	my ($compo,$source) = @_;

	foreach my $itf (sort(keys %{$components{$compo}{output_connection}}))
	{
		return 1 if (($components{$compo}{output_connection}{$itf}->{type} eq "empty") &&
					 ($components{$compo}{output_connection}{$itf}->{destination} eq "$source"));
	}

	return 0;
}


sub list_components {

	foreach my $compo (sort(keys %components))
	{
		print "-------------------------------\n$components{$compo}{name} [$compo] :\n";
		if($components{$compo}{output_connection})
		{
			foreach my $itf (sort(keys %{$components{$compo}{output_connection}}))
			{
				print "---  $itf => [$components{$compo}{output_connection}{$itf}->{destination}] ($components{$compo}{output_connection}{$itf}->{type})\n";
			}
		}

		if($components{$compo}{input_connection})
		{
			foreach my $itf (sort(keys %{$components{$compo}{input_connection}}))
			{
				print "---  $itf <= [$components{$compo}{input_connection}{$itf}->{source}] ($components{$compo}{input_connection}{$itf}->{type})\n";
			}
		}
		print "\n";
	}
}

sub generate_graph
{
	my $graph = GraphViz->new(rankdir  => 'LR',
							  label => "NMF graph ($output_graph)",
							  ratio => 'compress',
							  nodesep => '0.05',
							  splines  => 'ortho',
							  node => {shape    => 'box',
									   fontsize => '7',
									   fontname => 'arial',
									   height  => '0.05',
									   width   => '0.05'});

	foreach my $compo (sort(keys %components))
	{
		my $label = "$components{$compo}{name}\n($compo)";
		my $shape = "box";
		my $style = "rounded";
		my $color = "black";

		next if $components{$compo}{name} =~ /^_(st|sk|ev)_/;
		next if defined($components{$compo}{ignore});

		# First add connection (so that we can check for record)
		if($components{$compo}{output_connection})
		{
			foreach my $itf (sort(keys %{$components{$compo}{output_connection}}))
			{
				my $dest_this    = $components{$compo}{output_connection}{$itf}->{destination};
				my $edge_style   = "";
				my $display      = $wanted_interfaces{$components{$compo}{output_connection}{$itf}->{type}}->{display};
				my $asynchronous = $components{$compo}{output_connection}{$itf}->{asynchronous} || "";
				my $edge_color   = "black";

				if(defined($components{$compo}{output_connection}{$itf}->{asynchronous}))
				{
					$edge_style = "dashed";
				}

				if($display eq "record")
				{
					$label .= "| $components{$dest_this}{name}\n($dest_this)";
					$shape  = "record";
					next;
				}
				elsif($display eq "compo_filled")
				{
					$style.=",filled";
					$color = $wanted_interfaces{$components{$compo}{output_connection}{$itf}->{type}}->{fill_color};
					next;
				}
				if($display eq "none")
				{
					next;
				}
				elsif($display eq "color")
				{
					$edge_color = $wanted_interfaces{$components{$compo}{output_connection}{$itf}->{type}}->{color};
				}

				$graph->add_edge($compo => $dest_this,
								 style  => $edge_style,
								 color  => $edge_color);
			}
		}

		$graph->add_node("$compo",
						 label => "$label",
						 style => $style,
						 shape => $shape,
						 fillcolor => $color,
						 fontname => "arial",
						 fontsize => "9"
			);
	}

#		open TMP, ">nmf_graph.dot" or die "DEAD";
#		print TMP $graph->as_debug;
#		close TMP;
	$graph->as_png($output_graph);
}
