ARG BASE=cginternals/rendering-base
ARG BASE_DEV=cginternals/rendering-base:dev
ARG PROJECT_NAME=eglheadless
ARG GLBINDING_DEPENDENCY=cginternals/glbinding:latest
ARG GLESBINDING_DEPENDENCY=cginternals/glesbinding:latest
ARG EGLBINDING_DEPENDENCY=cginternals/eglbinding:latest

# DEPENDENCIES

FROM $GLBINDING_DEPENDENCY AS glbinding

FROM $GLESBINDING_DEPENDENCY AS glesbinding

FROM $EGLBINDING_DEPENDENCY AS eglbinding

# BUILD

FROM $BASE_DEV as build

ARG PROJECT_NAME
ARG COMPILER_FLAGS="-j 4"

RUN apt install -y --no-install-recommends libegl-dev pkgconf libgmock-dev

COPY --from=glbinding $WORKSPACE/glbinding $WORKSPACE/glbinding
COPY --from=glesbinding $WORKSPACE/glesbinding $WORKSPACE/glesbinding
COPY --from=eglbinding $WORKSPACE/eglbinding $WORKSPACE/eglbinding

ENV glbinding_DIR="$WORKSPACE/glbinding"
ENV glesbinding_DIR="$WORKSPACE/glesbinding"
ENV eglbinding_DIR="$WORKSPACE/eglbinding"
ENV eglheadless_DIR="$WORKSPACE/$PROJECT_NAME"

WORKDIR $WORKSPACE/$PROJECT_NAME

ADD cmake cmake
ADD data data
ADD deploy deploy
ADD docs docs
ADD source source
ADD CMakeLists.txt CMakeLists.txt
ADD configure configure
ADD $PROJECT_NAME-config.cmake $PROJECT_NAME-config.cmake
ADD LICENSE LICENSE
ADD README.md README.md
ADD AUTHORS AUTHORS

RUN ./configure
RUN CMAKE_OPTIONS="-DOPTION_BUILD_TESTS=On" ./configure
RUN cmake --build build -- $COMPILER_FLAGS

# INSTALL

FROM build as install

ARG PROJECT_NAME

WORKDIR $WORKSPACE/$PROJECT_NAME

RUN CMAKE_OPTIONS="-DCMAKE_INSTALL_PREFIX=$WORKSPACE/$PROJECT_NAME-install" ./configure
RUN cmake --build build --target install

# DEPLOY

FROM $BASE AS deploy

ARG PROJECT_NAME

RUN apt install -y qt5-default

COPY --from=build $WORKSPACE/glbinding $WORKSPACE/glbinding
COPY --from=build $WORKSPACE/glesbinding $WORKSPACE/glesbinding
COPY --from=build $WORKSPACE/eglbinding $WORKSPACE/eglbinding

COPY --from=install $WORKSPACE/$PROJECT_NAME-install $WORKSPACE/$PROJECT_NAME

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORKSPACE/glbinding/lib
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORKSPACE/glesbinding/lib
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORKSPACE/eglbinding/lib
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$WORKSPACE/$PROJECT_NAME/lib
