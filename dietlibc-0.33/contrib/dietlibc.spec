%define name	dietlibc
%define version	20010308
%define release	1mdk

Summary:	C library optimized for size
Name:		%{name}
Version:	%{version}
Release:	%{release}
Copyright:	GPL
Group:		Development/Other
BuildRoot:	%{_tmppath}/%{name}-%{version}-build
Requires:	common-licenses

Source0:	http://www.fefe.de/dietlibc/%{name}-%{version}.tar.bz2
Patch0:		dietlibc-20010308-install-includes.patch.bz2

%description
Small libc for building embedded applications.

%package devel
Group:          Development/C
Summary:        Development files for dietlibc
Requires:       %name = %version-%release

%description devel
Small libc for building embedded applications.

%prep
%setup
%patch0 -p1

%build
%make "CFLAGS=-march=i586 -fomit-frame-pointer -Os"

%install
make prefix=%{_prefix} INSTALLPREFIX=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README THANKS CAVEAT BUGS AUTHOR
%dir %{_prefix}/dietlibc

%files devel
%defattr(-,root,root)
%doc CHANGES TODO
%{_prefix}/dietlibc/*
%{_libdir}/*

%changelog
* Thu Mar  8 2001 Jeff Garzik <jgarzik@mandrakesoft.com> 20010308-1mdk
- first mdk contribs version

