%define name xmlcopyeditor
%define version 1.2.0.8
%define release 1
%define author Gerald Schmidt <gnschmidt@users.sourceforge.net>
%define prefix /usr/local

Name: %{name}
Version: %{version}
Release: %{release}
Prefix: %{prefix}
Packager: %{author}
Source0: %{name}-%{version}.tar.gz
Group: Applications/Editors
License: GPL
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
URL: http://xml-copy-editor.sourceforge.net
Vendor: XML Copy Editor Project
Summary: XML editor

%description
XML Copy Editor is a fast, free, validating XML editor.
%prep
%setup -q

%build
./configure --with-wx-config="wx-config --unicode" --datadir="%{prefix}/share"
make
strip src/xmlcopyeditor
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{prefix}/bin/xmlcopyeditor
%{prefix}/share/xmlcopyeditor/*
/usr/share/applications/xmlcopyeditor.desktop
/usr/share/pixmaps/xmlcopyeditor.png
