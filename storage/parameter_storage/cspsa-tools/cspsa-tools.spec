# ST-Ericsson spec file

Name:    cspsa-tools
Summary: Crash Safe Parameter Storage Area (CSPSA) Tools
Group:   System/Daemons
License: Proprietary
URL:     http://www.stericsson.com
Version: 0.1
Release: 1

Source0: %{name}-%{version}.tar.gz

BuildRequires: libcspsa-devel

Requires(post): /sbin/service
Requires(post): /sbin/chkconfig
Requires(postun): /sbin/service
Requires(postun): /sbin/chkconfig

%description
CSPSA (Crash Safe Parameter Storage Area) tools is a set of tools to be used
with CSPSA.

%package -n cspsa-hosttools
License: Proprietary
Summary: CSPSA host tools
Group:   System/Libraries
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires: cspsa = %{version}-%{release}

%description -n cspsa-hosttools
CSPSA host tools.

%prep
%setup -q

%build
make DISABLE_QSTORE_TARGET_INIT_AGENT=1 %{?_smp_mflags}

%install

#cspsa2nwm
install -d %{buildroot}%{_sbindir} %{buildroot}%{_sysconfdir}/rc.d/init.d/
install cspsa2nwm %{buildroot}%{_sbindir}
install cspsa-tools.init %{buildroot}%{_sysconfdir}/rc.d/init.d/cspsa-tools

#PC tools
install -d %{buildroot}%{_bindir}
install gdflist %{buildroot}%{_bindir}
install nwm2gdf %{buildroot}%{_bindir}
install gdf2pff %{buildroot}%{_bindir}
install pff2cspsa %{buildroot}%{_bindir}
install cspsalist %{buildroot}%{_bindir}

%clean
rm -rf %{buildroot}

# Remove init script on uninstall
%preun
if [ $1 = 0 ]; then
/sbin/service cspsa-tools stop > /dev/null 2>&1
/sbin/chkconfig --del cspsa-tools
fi

# Add init script on install
%post
/sbin/chkconfig --add cspsa-tools

%files
%defattr(-,root,root)
%{_sbindir}/cspsa2nwm
%{_sysconfdir}/rc.d/init.d/cspsa-tools

%files -n cspsa-hosttools
%defattr(-,root,root)
%{_bindir}/gdflist
%{_bindir}/nwm2gdf
%{_bindir}/gdf2pff
%{_bindir}/pff2cspsa
%{_bindir}/cspsalist
