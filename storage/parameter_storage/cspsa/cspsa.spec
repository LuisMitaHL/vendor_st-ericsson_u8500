# ST-Ericsson spec file

Name:    cspsa
Summary: Crash Safe Parameter Storage Area (CSPSA)
Group: 	 System/Daemons
License: BSD
URL:     http://www.stericsson.com
Version: 0.2
Release: 1

Source0: %{name}-%{version}.tar.gz

Requires(post): /sbin/service
Requires(post): /sbin/chkconfig
Requires(postun): /sbin/service
Requires(postun): /sbin/chkconfig

%description
CSPSA (Crash Safe Parameter Storage Area) is a user space library developed by
ST-Ericsson that provides access to the CSPSA at the end of the flash/eMMC used
for storing, for example, calibrated radio and security parameters. It utilizes
a raw MTD device or eMMC block device via libc to manipulate the contents in
the flash/eMMC memory. All concurrent users except one are blocked until access
to the underlying storage device is accessible.

The CSPSA implementation provides no security concept for the data stored.
Focus is on crash safeness and compatibility with production tools.

%package -n libcspsa
License: BSD
Summary: CSPSA shared library
Group:   System/Libraries
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires: cspsa = %{version}-%{release}

%description -n libcspsa
CSPSA shared library

%package -n libcspsa-devel
License: BSD
Summary: CSPSA shared library
Group:   Development/Libraries
Requires: libcspsa = %{version}-%{release}

%description -n libcspsa-devel
CSPSA shared library development files

%prep
%setup -q

%build
make %{?_smp_mflags}

%install
%makeinstall

#installing devel files
install -d %{buildroot}/%{_includedir} %{buildroot}/%{_libdir}
install -m 644 cspsa.h %{buildroot}/%{_includedir}
cp -d libcspsa.so %{buildroot}/%{_libdir}
#installing devel files for host tools
install -m 644 cspsa_core.h %{buildroot}/%{_includedir}
install -m 644 cspsa_ll.h %{buildroot}/%{_includedir}
install -m 644 cspsa_ll_file.h %{buildroot}/%{_includedir}
install -m 644 cspsa_plugin.h %{buildroot}/%{_includedir}
install -m 644 libcspsa-core.a %{buildroot}/%{_libdir}
install -m 644 libcspsa-ll.a %{buildroot}/%{_libdir}

%clean
rm -rf %{buildroot}

# Remove init script on uninstall
%preun
if [ $1 = 0 ]; then
/sbin/service cspsa stop > /dev/null 2>&1
/sbin/chkconfig --del cspsa
fi

# Add init script on install
%post
/sbin/chkconfig --add cspsa

%post -n libcspsa -p /sbin/ldconfig
%postun -n libcspsa -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_sbindir}/cspsa-server
%{_sbindir}/cspsa-cmd
%{_sysconfdir}/rc.d/init.d/cspsa
%config %{_sysconfdir}/cspsa.conf

%files -n libcspsa
%defattr(-,root,root)
%{_libdir}/libcspsa.so.*

%files -n libcspsa-devel
%defattr(-,root,root)
%{_includedir}/*.h
%{_libdir}/libcspsa.so
%{_libdir}/libcspsa-core.a
%{_libdir}/libcspsa-ll.a
