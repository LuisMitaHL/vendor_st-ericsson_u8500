#/*
# * =====================================================================================
# *
# *       Filename: Colors.pm
# *
# *    Description:  This package allows the use of colors in shells in a convenient way
# *
# *        Version:  1.0
# *        Created:  01/01/2005
# *       Revision:  none
# *
# *         Author:  Steven HUET (SH), steven.huet@stericsson.com
# *        Company:  STEricsson, Grenoble
# *
# * =====================================================================================
# */
package Colors;
use Term::ANSIColor;

require Exporter;
use Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(blue green red magenta yellow bold);

sub bold {
    my $arg = shift;
    print colored($arg,"bold") . "\n";
}
sub yellow {
    my $arg = shift;
    print colored($arg,"yellow") . "\n";
}
sub blue {
    my $arg = shift;
    print colored($arg,"bold blue") . "\n";
}
sub red {
    my $arg = shift;
    print colored($arg,"bold red") . "\n";
}
sub green {
    my $arg = shift;
    print colored($arg,"green") . "\n";
}
sub magenta {
    my $arg = shift;
    print colored($arg,"bold magenta") . "\n";
}
