![Build project](https://github.com/Ericsson/CodeCompass/workflows/Build%20project/badge.svg)
![Tarball packaging](https://img.shields.io/gitlab/pipeline/codecompass/CodeCompass/master?gitlab_url=https%3A%2F%2Fgitlab.inf.elte.hu&label=Tarball%20packaging)
[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/Ericsson/CodeCompass/badge)](https://securityscorecards.dev/viewer/?uri=github.com/Ericsson/CodeCompass)

CodeCompass
===========

CodeCompass is a pluginable code comprehension tool.

<img src="https://raw.githubusercontent.com/Ericsson/codecompass/master/webgui/images/logo.png" width="250px;"/>

**:bulb: Live demo on LLVM source code available [here](https://codecompass.zolix.hu/#wsid=llvmmaster)**

## Screenshots

<img src="https://raw.githubusercontent.com/Ericsson/codecompass/master/webgui/images/home.png" height="100px" /> <img src="https://raw.githubusercontent.com/Ericsson/codecompass/master/webgui/images/codebites.png" height="100px" /> <img src="https://raw.githubusercontent.com/Ericsson/codecompass/master/webgui/images/infotree.png" height="100px" /> <img src="https://raw.githubusercontent.com/Ericsson/codecompass/master/plugins/cpp/webgui/images/cpp_function_call_diagram.png" height="100px" /> <img src="https://raw.githubusercontent.com/Ericsson/codecompass/master/plugins/cpp/webgui/images/cpp_detailed_class_diagram.png" height="100px" />

Features
--------

- User friendly web UI
- Fast navigation among source code elements
- Several languages supported
- Deep parsing for C, C++, Java and more is coming
- Many diagrams: call path, inheritance, aggregation, CodeBites, etc.
- Scalable: Quick response time even for large (100Mb) source code base

Documentations
--------
- [Backend user guide](doc/usage.md)
- [Frontend user guide](doc/webgui.md)
- [Docker guide](docker/README.md)

Development
--------
- [Building CodeCompass](doc/deps.md)
- [Frontend development guide](webgui-new/README.md)
- [Coding conventions](doc/coding_conventions.md)
