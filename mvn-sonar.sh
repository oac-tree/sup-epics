#!/bin/bash

#+======================================================================
#+======================================================================

mvn clean test -Dcoverage
# Valgrind seems to conflict with the softIoc used during unit tests:
# mvn clean test -Dcoverage -Dvalgrind
mvn sonar -DupdateSonar -DanalysisMode=publish -Dlogin=f212cb1196e89c8984098d31a4fa6c9a43cc7d07
