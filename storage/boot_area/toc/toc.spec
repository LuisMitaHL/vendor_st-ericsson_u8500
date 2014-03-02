#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2011 ST-Ericsson SA                                   *
# *                                                                      *
# *  This software is released under the terms of the BSD-style          *
# *  license accompanying TOC.                                           *
# *                                                                      *
# *  Author: Mikael Gullberg <mikael.xx.gullberg AT stericssom.com >     *
# *                                                                      *
# ************************************************************************/

Name:    toc
Summary: Cmd-line tool for handling TOC-partitions.
Group:   System/Libraries
License: BSD
URL:     http://www.stericsson.com
Version: 0.1
Release: 1

Source0: %{name}-%{version}.tar.gz

%description
Cmd-line tool for handling the content of partitoons decribed by the
TOC-partition format. It is possible to read and write the content of the
different partitions using the command line tool. Updates made to the partition
content are not power failure safe.

%prep
%setup -q

%build
make all %{?_smp_mflags}

%install
install -d %{buildroot}%{_sbindir}
install tocparser %{buildroot}%{_sbindir}

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%{_sbindir}/tocparser
