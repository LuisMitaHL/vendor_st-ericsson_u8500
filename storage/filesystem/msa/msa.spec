# ST-Ericsson spec file

Name:    msa
Summary: Modem Storage Agent (MSA)
Group:   System/Daemons
License: Proprietary
Url:     http://www.stericsson.com
Version: 0.1
Release: 4

Source: %{name}-%{version}.tar.gz

Requires(post): /sbin/ldconfig
Requires(post): /sbin/service
Requires(post): /sbin/chkconfig
Requires(postun): /sbin/ldconfig
Requires(postun): /sbin/service
Requires(postun): /sbin/chkconfig

%description
Modem Storage Agent (MSA) interprets the FSA (File System Access) commands in RPC and
converts these to the corresponding accesses to file system and then returns the
appropriate response back through RPC.

Only FSA commands are supported for RPC even though RPC could potentially contain
other kind of commands.

MSA is one part in the chain to provide some modems with non volatile memory access,
such as the file system

%prep
%setup -q

%build
make %{?_smp_mflags} BUILD=release msa

%install
%makeinstall

%clean
rm -rf %{buildroot}

# Remove init script on uninstall
%preun
if [ $1 = 0 ]; then
/sbin/service msa stop > /dev/null 2>&1
/sbin/chkconfig --del msa
fi

# Add init script on install
%post
/sbin/chkconfig --add msa

%files
%defattr(-,root,root)
%{_sbindir}/msa
%{_sysconfdir}/rc.d/init.d/msa
