Name:           polecat
Version:        0.2.0
Release:        1%{?dist}
Summary:        simple wine version manager

License:        MIT
URL:            https://github.com/Jan200101/polecat
Source0:        %{url}/archive/%{version}.tar.gz#/%{name}-%{version}.tar.gz

BuildRequires:  libcurl-devel
BuildRequires:  json-c-devel
BuildRequires:  libarchive-devel
BuildRequires:  cmake
BuildRequires:  gcc gcc-c++
# DXVK functionality requires wine
Requires:       wine

%description
polecat is wine manager that manages, installs and downloads wine for you

%prep
%autosetup

%build
%cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo
%cmake_build

%install
%cmake_install

%check
%ctest

%files
%license LICENSE
%{_bindir}/%{name}

%changelog
* Thu May 05 2022 Jan Drögehoff <sentrycraft123@gmail.com> - 0.2.0-1
- Update to 0.2.0

* Mon Nov 23 18:39:58 CET 2020 Jan Drögehoff <sentrycraft123@gmail.com> - 0.1.9-1
- Update to 1.9

* Sun Nov 22 22:21:50 CET 2020 Jan Drögehoff <sentrycraft123@gmail.com> - 0.1.8-1
- Initial spec
