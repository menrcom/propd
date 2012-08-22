Name:           propagated
Version:        0.1.1
Release:        1%{?dist}
Summary:        Multicast information exchange toolkit

Group:          Applications/Internet
License:        GPL
URL:            http://clodo.ru/
Source0:        %{name}-%{version}.tar.gz
Buildroot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

#BuildRequires:
#Requires:       
Requires(pre):      /sbin/ip /sbin/route
#Requires(post):     
#Requires(preun):    
#Requires(preun):    

%description
Multicast daemon, get and set tools.

%prep
%setup -q

%build
autoreconf -i
%configure --enable-debug
make

%install
make DESTDIR=$RPM_BUILD_ROOT install
install -m 0755 -g root -o root -d $RPM_BUILD_ROOT/etc/init.d
install -m 0755 -g root -o root -d $RPM_BUILD_ROOT/etc/sysconfig
install -m 0755 -g root -o root $RPM_BUILD_ROOT/usr/share/propagated/SLES_init-propagated $RPM_BUILD_ROOT/etc/init.d/propagated
install -m 0644 -g root -o root $RPM_BUILD_ROOT/usr/share/propagated/sysconfig-propagated $RPM_BUILD_ROOT/etc/sysconfig/propagated

%files
%defattr(0755,root,root)
%{_sbindir}
%{_bindir}
/etc/init.d/propagated
/usr/share/propagated/SLES_init-propagated
%defattr(0644,root,root)
%config /etc/sysconfig/propagated
%config /usr/share/propagated/sysconfig-propagated
