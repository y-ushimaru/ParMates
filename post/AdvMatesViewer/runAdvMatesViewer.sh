#!/bin/bash

JAVA_COMMAND=java
MAX_HEAP_IN_MB=1000
JAVA_OPTION="-Djava.library.path=lib/jogl/linux/amd64"
JAVA_CLASSES="-cp AdvMatesViewer.jar:lib/ant.jar:lib/miglayout-4.0.jar:lib/jogl/gluegen-rt.jar:lib/jogl/jogl.jar:"
MAIN_CLASS=jp.ac.u_tokyo.t.q.adventure.mates.AdvMatesViewerMain

${JAVA_COMMAND} -Xmx${MAX_HEAP_IN_MB}M ${JAVA_OPTION} ${JAVA_CLASSES} ${MAIN_CLASS}
