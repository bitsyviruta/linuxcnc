#                                               -*- Autoconf -*-
# Procesar este archivo con autoconf para producir el script configure. 
# 
# Descripcion: configure.ac 
#   Autonconf produce ./configure desde este archivo 
#   que se usa para averiguar varios programas/dependencias 
#   uso: autoconf (nota: autogen.sh incluye la llamada a autoconf)
#
# Authors: paul_c alex_joni jepler
# License: GPL Version 2
# System: Linux
#    
# Copyright (c) 2004 All rights reserved.
#
# Last change:
# This file is released under GPL, refer to docs/README for details
#
# Este archivo está adaptado para reemplazar el antiguo script ./configure
# por lo tanto incluye partes de ese script

# N.T. Este archivo esta sobrecomentado en su version ES ya que se usó para estudiar
# la construccion de LinuxCNC

##############################################################################
# Sección 1
# Inicialización autoconf: 
# 	Lanza diversas macros m4 que configuran o encuentran configuraciones de cosas de interes.
# 		macro versión mínima de Autoconf.
# 		INIT obligatorio. Nombre del paquete, versión, correo electrónico para informe de errores.
# 		Asegurar existencia del directorio de código fuente, comprobando que existe emc/motion/motion.c
# 		Test del nivel superior del directorio de fuentes. Error en su caso.
# 		Buscar nombre compilador C++ 
# Chequear boost/lockfree/queue.hpp
# Incluir...m4_include([m4/ax_cxx_compile_stdcxx_11.m4]) 
# Opcional, para C++ version 11, esto puede producir warnings 
# Definicion de macro _PYTHON_SYSCONFIG 
# Definicion de macro AX_BOOST_PYTHON_EMC 
# Para la libreria compartida boost::python]) 
# Para imprimir mensajes de chequeo en terminal....... 
# Soporte para algunos de las opciones estandar de configure para directorios
# Prefijo de instalacion..... 
 
# Ver si se quiere “run in place”
# Nombre internacionalizado del paquete
# Valor de version, del  archivo VERSION 
# Buscar path absoluto para grep 
# Buscar path absoluto para pkg-config
# Llamada a procedimiento remoto. check para rpc/rpc.h
##############################################################################
# macro version minima de Autoconf
AC_PREREQ(2.53
# macro Cada script de configuración debe llamar primero a AC_INIT(package, version, [bug-report], [tarname], [url])
# Usa macro m4 esyscmd() para extraer version del archivo VERSION
# m4_normalize() elimina espacios y tabs iniciales y finales, secuencias barra-nuevalínea, reemplaza espacios múltiples, 
# tabs y nuevas-líneas con un solo espacio.
AC_INIT(LinuxCNC, m4_normalize(esyscmd([cat ../VERSION])), emc-developers@lists.sourceforge.net)
# macro que comprueba la existencia de este archivo para asegurarse de que el directorio contiene el código fuente.
AC_CONFIG_SRCDIR(emc/motion/motion.c)
if test "$srcdir" != "."; then
    AC_MSG_ERROR([La compilacion fuera de /src no esta soportada])
fi

# macro Chequeo del compilador C++ a usar, dando valores a variables entorno CXX, GXX y CXXFLAGS
AC_PROG_CXX

# Chequear boost/lockfree/queue.hpp
AC_LANG_PUSH([C++])
# macro check system header file.  AC_CHECK_HEADER (header-file, [action-if-found], [action-if-not-found], [includes])
AC_CHECK_HEADER([boost/lockfree/queue.hpp],
    [], [RTAPI_APP_CFLAGS="$RTAPI_APP_CFLAGS -I external"])
AC_LANG_POP([C++]) 
AC_SUBST([RTAPI_APP_CFLAGS])

# Incluir...m4_include([m4/ax_cxx_compile_stdcxx_11.m4])
# Verifica la cobertura de idioma base en el compilador para
# el estándar C++ 11; si es necesario, agrega switchs a CXXFLAGS para habilitar el soporte.
m4_include([m4/ax_cxx_compile_stdcxx_11.m4])
# make this optional - it will likely only result in warnings
AX_CXX_COMPILE_STDCXX_11(noext,optional)

# definicion de macro
## _PYTHON_SYSCONFIG(expression, [action-if-succeeded], [action-if-failed])
##
m4_define([_PYTHON_SYSCONFIG],
[dnl
py_val=`$PYTHON -c "dnl
[import sys; from distutils import sysconfig; sys.stdout.write]dnl
([sysconfig.]$1 or '')" 2>&AS_MESSAGE_LOG_FD`
AS_IF([test "[$]?" -eq 0 && test "x$py_val" != x], [$2], [$3])[]dnl
])

# adaptado de AX_BOOST_PYTHON_MURRAYC
# http://ftp.gnome.org/pub/GNOME/sources/glom/1.19/
# definicion de macro AX_BOOST_PYTHON_EMC
m4_define([AX_BOOST_PYTHON_EMC],
[
AC_LANG_PUSH([C++])
saved_CPPFLAGS=$CPPFLAGS
saved_LIBS=$LIBS
# Tenga en cuenta que esto requiere PYTHON_CPPFLAGS
# Tenga en cuenta que esto espera que boost/ se encuentre en algún nivel superior, como /usr/include/
# No pudimos buscar otra cosa porque no hay ningún archivo pkg-config que nos diga dónde está
CPPFLAGS="$PYTHON_CPPFLAGS $saved_CPPFLAGS"
AC_CACHE_CHECK([Si esta disponible cabecera Boost::Python],
               [ac_cv_boost_python],
               [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <boost/python/object_fwd.hpp>
using namespace boost::python;
]], [])],
    [ac_cv_boost_python=yes], [ac_cv_boost_python=no])])

AS_IF([test "x$ac_cv_boost_python" = xyes],
[
  BOOST_PYTHON_LIBS=
  ax_python_lib=boost_python
  AC_ARG_WITH([boost-python],
    AS_HELP_STRING(
        [--with-boost-python],
        [Especifique la biblioteca compartida boost python a usar. por
         ejemplo, --with-boost-python = boost_python-py25. boost-python por defecto.
         Si usa esto, entonces probablemente debería configurar PYTHON
         también, para evitar el uso de múltiples versiones de python.]
    ),
  [if test "x$with_boost_python" != xno; then
    ax_python_lib=$with_boost_python
    ax_boost_python_lib=boost_python-$with_boost_python
  fi])
  AC_MSG_CHECKING([para libreria compartida boost::python])
  for ax_lib in "$ax_python_lib" "$ax_boost_python_lib" boost_python
  do
    # Esto require LIBPYTHON desde configure.ac
    LIBS="$saved_LIBS -l$LIBPYTHON -l$ax_lib"
    AC_LINK_IFELSE(
      [AC_LANG_PROGRAM([[#include <boost/python/object_fwd.hpp>]], [[boost::python::object *test_object;]])],
      [BOOST_PYTHON_LIBS="-l$ax_lib"; break])
  done
  AS_IF([test "x$BOOST_PYTHON_LIBS" != x], [ax_result=$BOOST_PYTHON_LIBS], [ax_result=])
  AC_MSG_RESULT([$ax_result])
])
CPPFLAGS=$saved_CPPFLAGS
LIBS=$saved_LIBS
AC_LANG_POP([C++])
AS_IF([test "x$ac_cv_boost_python" = xyes && test "x$BOOST_PYTHON_LIBS" != x],
      [AC_DEFINE([HAVE_BOOST_PYTHON], [1],
                 [define si los encabezados y la biblioteca Boost::Python están disponibles])],
      [AC_MSG_ERROR([[boost::python es necesario para compilar $PACKAGE_NAME]])])
AC_SUBST([BOOST_PYTHON_LIBS])
])
# Fin macro AX_BOOST_PYTHON_EMC

# nivel superior
(build toplevel)
BUILD_TOPLEVEL="$(cd ..; pwd -P)"
AC_MSG_RESULT($BUILD_TOPLEVEL)

AC_CONFIG_HEADER(config.h)

# Soporte pra algunas de las opciones de configuración estándar para directorios
# Si no se indica prefix, se asume instalacion RIP 
# guardar prefix en var AUTODIR
AC_PREFIX_DEFAULT(run-in-place)

AC_MSG_CHECKING(prefix de instalacion)
if test "x$prefix" = "xNONE"; then
  RUN_IN_PLACE=yes
  AC_MSG_RESULT(run in place)
  prefix=${BUILD_TOPLEVEL}
else
  RUN_IN_PLACE=no
  AC_MSG_RESULT($prefix)
fi
AC_SUBST(AUTODIRS)

# internationalisation package name:
# variables para nombre del paquete (PACKAGE) y version (EMC2VERSION)
PACKAGE="linuxcnc"
AC_SUBST([PACKAGE])

if test -f ../VERSION; then
    EMC2VERSION="$(cat ../VERSION)"
else
    EMC2VERSION="Indeterminada"
fi
AC_SUBST([EMC2VERSION])

# variables GREP y EGREP, para chequeo paths de grep y egrep
AC_PATH_PROG(GREP, grep)
AC_PATH_PROG(EGREP, egrep)

#
# check for pkg-config
# El programa pkg-config se usa para recuperar información sobre las bibliotecas instaladas en el sistema.
# variable PKG_CONFIG para su path, si existe. Error si no esta instalado.
AC_PATH_PROG([PKG_CONFIG], [pkg-config])
if test -z "$PKG_CONFIG"; then
  AC_MSG_ERROR([se requiere pkg-config para compilar LinuxCNC])
fi

# Llamada a procedimiento remoto
# check para rpc/rpc.h
#
AC_CHECK_HEADER([rpc/rpc.h],[], [
  if ! test -z "$PKG_CONFIG libtirpc"; then

    LIBTIRPC_CFLAGS=`pkg-config libtirpc --cflags`
    AC_SUBST([LIBTIRPC_CFLAGS])

    LIBTIRPC_LIBS=`pkg-config libtirpc --libs`
    AC_SUBST([LIBTIRPC_LIBS])

    CFLAGS+=" `pkg-config libtirpc --cflags` "
  fi

  # Actualizar path
  AC_CHECK_LIB([tirpc], [get_myaddress], [], [
    AC_MSG_ERROR([No encontré tirpc])
  ])

  # Limpiar cached check
  $as_unset ac_cv_header_rpc_rpc_h
  AC_CHECK_HEADER([rpc/rpc.h],[],[
    AC_MSG_ERROR([glibc rpc.h desaparecido. por favor, instale libtirpc])
  ])
])


##############################################################################
# Seccion 2
# opcion --enable-non-distributable => NONDISTRIBUTABLE=true 
# Chequeo para RealTime ( RTAI, o RT-Linux) 
# Subseccion 2.1 - argumentos ./configure (--with-realtime)
# Subseccion 2.2 – Si tiempo real no especificado, RTDIR debe ser encontrado por busqueda
# Subseccion 2.3 - RTS(RT-script) encontrado, se consulta para valores
##############################################################################

# setup de algunos valores por defecto
RTDIR=""
RTAI=""
RTPREFIX=""
KERNELDIR=""
RTFLAGS=""

# opcion --enable-non-distributable => NONDISTRIBUTABLE=true
NONDISTRIBUTABLE=false
AC_ARG_ENABLE(non-distributable,
    AS_HELP_STRING(
        [--enable-non-distributable],
        [Construir LinuxCNC, incluso si un problema de licencia haría que los binarios resultantes no se distribuyan bajo los términos de la licencia de LinuxCNC.]),
    [
        case "$enableval" in
        (yes) NONDISTRIBUTABLE=true ;;
        esac
    ])

##############################################################################
# Subseccion 2.1
# Chequeo de argumentos de ./configure
# accepta --with-realtime=<path> desde linea de comandos
# termina configurando real time string RTS
##############################################################################

# si el usuario usa --enable-simulator, avisar obsolencia, => RTS=uspace
RTDIR=""
AC_ARG_ENABLE(simulator,
    AS_HELP_STRING(
        [--enable-simulator],
        [Obsoleto!  Use --with-realtime=uspace en su lugar.]
    ),
    [
        case "$enableval" in
        Y*|y*)
            RTS=uspace
            AC_MSG_WARN([--enable-simulator es obsoleto. Use --with-realtime=uspace en su lugar.])
        ;;
        esac
    ])

# si el usuario usa --with-realtime sin path ni uspace, avisarlo
AC_ARG_WITH(realtime,
    AS_HELP_STRING(
        [--with-realtime=PATH],
        [Path donde RTAI esta instalado, o "uspace" para espacio de usuario POSIX.]
    ),
    [   case "$withval" in
	"" | y | ye | yes | n | no)
	    AC_MSG_ERROR([Debe indicar un path o "uspace" con --with-realtime.]) ;;
	*)
            RTS="$withval" ;;
        esac
        ])
AC_SUBST([RTS])

##############################################################################
# Subseccion 2.2
# 1. Si el usuario ha especificado un RT, se debe verificar su validez.
# Esto se hace comprobando la ubicacion predeterminada de determinado script.
# 2. Si el usuario no ha especificado explícitamente RT, se tiene que 
# encontrar por búsqueda. 
# Para hacer eso, el código buscará un script específico de RT:
# rtai-config para RTAI 3.x
# TODO para versiones mas recientes de RTAI
##############################################################################

# en este punto, si RTS está vacío, se necesita encontrar RT en localizaciones probables
AC_MSG_CHECKING([para rtai-config])
if test -z "$RTDIR"; then
  DIRS="/usr/realtime-`uname -r` /usr/realtime /usr/realtime* /usr /usr/src/rtai*"
else
  DIRS="$RTS"
fi
  RTAI_CONFIG=none
  for location in $DIRS; do
    for subdir in . scripts bin; do
    for signature in rtai; do
      candidate=$location/$subdir/$signature-config
      if test "$RTAI_CONFIG" = "none" -a -x $candidate; then
        RTAI_CONFIG=$candidate
      fi
    done
  done
done

AC_MSG_RESULT([$RTAI_CONFIG])
AC_SUBST([RTAI_CONFIG])

AC_PATH_PROG([XENOMAI_CONFIG], [xeno-config], [none])
AC_SUBST([XENOMAI_CONFIG])

# API(s) realtime.

# Para RTAI:
# El módulo LX (Linux) RT (RealTime), implementa servicios para poner a disposición 
# de los procesos de Linux las funciones del scheduler RTAI, de modo que es posible 
# una implementación totalmente simétrica de servicios en tiempo real.
# Eso significa que puede compartir memoria, enviar mensajes, usar semáforos y temporizado
# Linux <-> Linux, Linux <-> RTAI y, naturalmente, RTAI <-> RTAI.

AC_MSG_CHECKING([para API(s) realtime a usar])
if test "$RTS" = uspace || test -z "$RTS" -a "$RTAI_CONFIG" = "none" ; then
  RTS=uspace
  if test "$RTAI_CONFIG" = "none" ; then
    USPACE_RTAI=
    CONFIG_USPACE_RTAI=n
    RTAI_LXRT_CFLAGS=
    RTAI_LXRT_LDFLAGS=
  else
    USPACE_RTAI=+lxrt
    CONFIG_USPACE_RTAI=y
    RTAI_LXRT_CFLAGS=`$RTAI_CONFIG --lxrt-cflags`
    RTAI_LXRT_LDFLAGS=`$RTAI_CONFIG --lxrt-ldflags`
    AC_DEFINE([USPACE_RTAI], [], [Define si uspace en tiempo real debería opcionalmente soportar RTAI LXRT])
  fi
  AC_SUBST([CONFIG_USPACE_RTAI])
  AC_SUBST([RTAI_LXRT_CFLAGS])
  AC_SUBST([RTAI_LXRT_LDFLAGS])

# Para Xenomai
  if test "$XENOMAI_CONFIG" = "none" ; then
    USPACE_XENOMAI=
    CONFIG_USPACE_XENOMAI=n
    XENOMAI_CFLAGS=
    XENOMAI_LDFLAGS=
  else
    USPACE_XENOMAI=+xenomai
    CONFIG_USPACE_XENOMAI=y
    XENOMAI_CFLAGS=`$XENOMAI_CONFIG --skin posix --cflags`
    XENOMAI_LDFLAGS=`$XENOMAI_CONFIG --skin posix --ldflags`
    AC_DEFINE([USPACE_XENOMAI], [], [Define si uspace en tiempo real debería opcionalmente soportar Xenomai])
  fi
  AC_SUBST([CONFIG_USPACE_XENOMAI])
  AC_SUBST([XENOMAI_CFLAGS])
  AC_SUBST([XENOMAI_LDFLAGS])

  AC_MSG_RESULT([$RTS$USPACE_RTAI$USPACE_XENOMAI])
# Fin API(s) realtime

# udev y libudev
  AC_MSG_CHECKING([si se habilita el acceso PCI al espacio de usuario])
  AC_ARG_ENABLE(userspace-pci,
      AS_HELP_STRING(
        [--disable-userspace-pci],
        [Deshabilitar el acceso PCI desde espacio de usuario (use esto si libudev no está disponible).],
      ),
      [
	case "$enableval" in
	(Y*|y*)
	    BUILD_UDEV=yes ;;
	(*)
	    BUILD_UDEV=no ;;
	esac
      ],
      [
        case "`uname -s`" in
        (*BSD*) BUILD_UDEV=no ;;
        (*) BUILD_UDEV=yes ;;
        esac
      ])
  AC_MSG_RESULT([$BUILD_UDEV])
  AC_SUBST([BUILD_UDEV])

  if test "$BUILD_UDEV" = "yes"; then
    AC_MSG_CHECKING([for libudev])
    if pkg-config libudev >/dev/null 2>&1; then
        LIBUDEV_VER=`pkg-config udev --modversion`
        AC_MSG_RESULT(yes - version [$LIBUDEV_VER])
        LIBUDEV_CFLAGS=`pkg-config libudev --cflags`
        LIBUDEV_LIBS=`pkg-config libudev --libs`
    else
      # imprimir un mensaje de error en la salida de error estándar y salir.
      AC_MSG_ERROR([libudev-dev no encontrada])
    fi
  else
    LIBUDEV_CFLAGS=-DNO_LIBUDEV
  fi
  AC_SUBST([LIBUDEV_CFLAGS])
  AC_SUBST([LIBUDEV_LIBS])
else
  RTS="$RTAI_CONFIG"
  AC_MSG_RESULT([$RTS])
fi


##############################################################################
# Subseccion 2.3 
# Tenemos ahora el script RT; es hora de consultarlo para algunos valores
# Nos puede proporcionar valores predeterminados como CC, CFLAGS, etc. 
# ver cada caso para más detalles
##############################################################################

RTARCH=`uname -m | sed s/i.86/i386/`
case $RTS in
*/rtai-config)
    RTPREFIX=rtai
    RTAI_VERSION=$($RTS --version)
    RTAI=$(echo $RTAI_VERSION | cut -f 1 -d .)
    RTDIR=`$RTS --prefix`
    RTDIR=$(cd $RTDIR ; pwd -P )
    RTFLAGS=`$RTS --module-cflags`
    RTFLAGS="$RTFLAGS -DRTAI=$RTAI"
    KERNELDIR=`$RTS --linux-dir`
    RTARCH=`$RTS --arch`
    ULAPI_CFLAGS=`$RTS --lxrt-cflags`
    ULAPI_LDFLAGS=`$RTS --lxrt-ldflags`
;;
uspace|uspace+*)
    RTPREFIX=uspace
    RTDIR=$(pwd -P)
    ULAPI_CFLAGS=""
    RTFLAGS="-DUSPACE"
    BUILD_SYS=uspace
    MODEXT=.so
    MODULE_DIR=${prefix}/lib/linuxcnc/modules
    ULAPI_LDFLAGS=""
esac

if test "$RTS" != uspace; then
    AC_MSG_CHECKING([para localizacion de cabeceras del kernel])
    AC_ARG_WITH(kernel-headers,
        AS_HELP_STRING(
            [--with-kernel-headers=PATH],
            [Localizacion de cabeceras del kernel.]
        ),
	[
	    KERNELDIR="$withval"
	    AC_MSG_RESULT(usando valor especificado: $KERNELDIR)
	],[
	    AC_MSG_RESULT(usando valor desde RTS: $KERNELDIR)
	])
fi

AC_SUBST([RTAI])
AC_SUBST([RTPREFIX])
AC_SUBST([KERNELDIR])
AC_SUBST([RTFLAGS])
AC_SUBST([RTDIR])
AC_SUBST([RTARCH])
AC_SUBST([ULAPI_CFLAGS])
AC_SUBST([ULAPI_LDFLAGS])

case "$RTPREFIX" in
uspace)
    AC_DEFINE(RTAPI_USPACE, [], [Sistema Realtime es userspace]) ;;
rtai)
    AC_DEFINE(RTAPI_RTAI, [], [Sistema Realtime es RTAI]) ;;
esac


EMC2_HOME=`(cd .. ; pwd)`
AC_SUBST([EMC2_HOME])

AC_DEFINE_UNQUOTED([RTDIR], "$RTDIR", [Directorio del sistema en tiempo real instalado.])

##############################################################################
# Seccion 3
# Chequeos adicionales
# Subseccion 3.1 - chequeo para CC (se debe usar el mismo que el usado para RT)
# Subseccion 3.2 - chequeo de version kernel, con parches RT
# Subseccion 3.3 - chequeo del dir de modulos, donde puedan ser instalados
# Subseccion 3.4 - chequeo para GTK
# Subseccion 3.5 - chequeo de utilidades precisas para compilar/correr linuxcnc
##############################################################################


##############################################################################
# Subseccion 3.1 - chequeo para version CC (se debe usar el mismo que el usado para RT)
##############################################################################
AC_MSG_CHECKING([para version CC])

if (test ${RTS##*/} = rtai-config ) ; then
  RTSCC=`$RTS --cc`
  AC_MSG_RESULT($RTSCC encontrado en rtai-config)
else
  AC_MSG_RESULT(no especificado)
fi

AC_PROG_CC([$RTSCC gcc clang])
AC_PROG_CPP()

if test -z "$CC"; then
  # imprimir mensajes de error en la salida de error estándar y salir.
  AC_MSG_ERROR([no encontrado cc aceptable])
elif ! test `$CC -dumpversion | cut -d '.' -f 1` -gt 2 ; then
  AC_MSG_ERROR([compiladores más antiguos que gcc 3.x ya no son soportados])
fi

##############################################################################
# El controlador hidraw proporciona una interfaz en bruto para Dispositivos de Interfaz Humana
# USB y Bluetooth (HID).
##############################################################################
AC_MSG_CHECKING([para usabilidad de linux/hidraw.h])
AC_TRY_LINK(
    [
#include <sys/ioctl.h>
#include <linux/hidraw.h>
long hidiocgrawinfo = HIDIOCGRAWINFO;
    ],
    [],
    [
	HIDRAW_H_USABLE=yes
        AC_DEFINE([HIDRAW_H_USABLE], [], [Define a 1 si linux/hidraw.h es usable y define HIDIOCGRAWINFO])
],
    [HIDRAW_H_USABLE=no])
AC_MSG_RESULT($HIDRAW_H_USABLE)
AC_SUBST([HIDRAW_H_USABLE])

#
# Check para rpc/rpc.h
#

AC_MSG_CHECKING([para usabilidad de rpc/rpc.h])
AC_TRY_LINK(
    [
#include <rpc/rpc.h>
    ],
    [
struct sockaddr_in *foo;
get_myaddress(foo);
    ],
    [
        AC_MSG_RESULT(yes)
    ],
    [
        AC_MSG_RESULT(no)
        AC_MSG_ERROR([inhabilitado para usar rpc.h])
    ]
)

##############################################################################
# check para libmodbus3; libreria para modbus. (https://libmodbus.org/)
##############################################################################

AC_ARG_WITH(
    [libmodbus],
    AS_HELP_STRING(
        [--with-libmodbus],
        [Especifique si desea compilar o no los controladores que utilizan libmodbus
        (por defecto "yes").]
    ),
    [WITH_LIBMODBUS=$withval],
    [WITH_LIBMODBUS=yes]
)

AS_IF(
    [test "x$WITH_LIBMODBUS" = "xyes"],
    [
        AC_MSG_CHECKING([para libmodbus3])
        if pkg-config libmodbus --atleast-version 3 >/dev/null 2>&1; then
            LIBMODBUS_VER=`pkg-config libmodbus --modversion`
            AC_MSG_RESULT(yes - version [$LIBMODBUS_VER])
            LIBMODBUS_CFLAGS=`pkg-config libmodbus --cflags`
            AC_SUBST([LIBMODBUS_CFLAGS])
            LIBMODBUS_LIBS=`pkg-config libmodbus --libs`
            AC_SUBST([LIBMODBUS_LIBS])
            AC_DEFINE(
                [HAVE_LIBMODBUS3],
                [yes],
                [Definir si los encabezados y la biblioteca libmodbus3 están disponibles.]
            )
            AC_SUBST(HAVE_LIBMODBUS3, yes)
        else
          AC_MSG_RESULT(no)
	  # imprimir un mensaje de error en la salida de error estándar y salir.
          AC_MSG_ERROR([libmodbus3 no encontrado!
instalar con "sudo apt-get install libmodbus-dev" o deshabilitar con
"configure --without-libmodbus"])
        fi
    ]
)


##############################################################################
# check para libusb-1.0. Biblioteca para la programación de aplicaciones que 
# usen USB sin necesidad de conocer el interior del núcleo Linux. (https://libusb.info/)
##############################################################################

AC_ARG_WITH(
    [libusb-1.0],
    AS_HELP_STRING(
        [--with-libusb-1.0],
        [Especifique si desea compilar o no los controladores que utilizan libusb-1.0
        (por defecto "yes").]
    ),
    [WITH_LIBUSB10=$withval],
    [WITH_LIBUSB10=yes]
)

AS_IF(
    [test "x$WITH_LIBUSB10" = "xyes"],
    [
        AC_MSG_CHECKING([for libusb-1.0])
        if pkg-config libusb-1.0 >/dev/null 2>&1; then
            AC_MSG_RESULT(yes)
            LIBUSB10_CFLAGS=`pkg-config libusb-1.0 --cflags`
            AC_SUBST([LIBUSB10_CFLAGS])
            LIBUSB10_LIBS=`pkg-config libusb-1.0 --libs`
            AC_SUBST([LIBUSB10_LIBS])
            AC_DEFINE(
                [HAVE_LIBUSB10],
                [yes],
                [Definir si los encabezados y la biblioteca libusb-1.0 están disponibles.]
            )
            AC_SUBST(HAVE_LIBUSB10, yes)
        else
          AC_MSG_RESULT(no)
	  # imprime un mensaje de error en la salida de error estándar y salir.
          AC_MSG_ERROR([libusb-1.0 no encontrada!
instalar con "sudo apt-get install libusb-1.0-0-dev" o deshabilitar con
"configure --without-libusb-1.0"])
        fi
    ]
)


##############################################################################
# Subsection 3.2 - comprobar la versión del kernel, con parches RT           #
#----------------------------------------------------------------------------#
# Comprobar los encabezados del kernel para encontrar la versión uts-kernel. #
# Esta debería ser una forma más confiable de determinar el kernel que       #
# se está utilizando para compilar, ya que `uname -r` solo devuelve la       #
# la versión del kernel que puede no coincidir con la version con la que     #
# el código es compilado.                                                    #
# UTS; deriva del nombre de la estructura pasada a la llamada al sistema     # 
# uname(): struct utsname ("UNIX Time-sharing System")                       # 
##############################################################################

if test $RTS '!=' uspace; then
AC_MSG_CHECKING([para cadena version kernel])

if test -e $KERNELDIR/include/linux/utsrelease.h; then
    VERSION_FILE=$KERNELDIR/include/linux/utsrelease.h
elif test -e $KERNELDIR/include/generated/utsrelease.h; then
    VERSION_FILE=$KERNELDIR/include/generated/utsrelease.h
else
    VERSION_FILE=$KERNELDIR/include/linux/version.h
fi
KERNEL_VERS=`$CC -E -dM ${VERSION_FILE} | grep UTS | cut -s -d '"' -f 2`

if test -z "$KERNEL_VERS"; then 
    # imprimir un mensaje de error en la salida de error estándar y salir. 
    AC_MSG_ERROR(Cadena de versión del kernel no encontrada)
fi

AC_MSG_RESULT([$KERNEL_VERS])
AC_SUBST([KERNEL_VERS])
AC_DEFINE_UNQUOTED([RTAPI_KERNEL_VERSION], "$KERNEL_VERS", [Versión requerida del kernel])

# prueba rápida para la extensión de módulo...
AC_MSG_CHECKING([para extension modulos kernel])
MODEXT=`$RTS --module-ext 2>/dev/null` || MODEXT=".o"
if test "x" = "x$MODEXT"; then
    MODEXT=".o"
fi
AC_MSG_RESULT([$MODEXT])

# Esta comprobación debería ver si se usa o no kbuild.
AC_MSG_CHECKING([kernel build system])
# FIXME FIXME FIXME
# de momento se asumirá en base a la extensión del módulo del kernel
EXT_RTFLAGS=""
BUILD_SYS="normal"

if test "x$MODEXT" = "x.ko" ; then
    BUILD_SYS="kbuild"
else
    EXT_RTFLAGS="-DEXPORT_SYMTAB"
fi

AC_ARG_WITH(kbuild,
    AS_HELP_STRING(
        [--with-kbuild],
        [Especifica si su kernel usa kbuild o no.]
    ),
    [
        case "$withval" in
        "" | y | ye | yes )
	    BUILD_SYS="kbuild"
          ;;
        n | no )
	    BUILD_SYS="normal"
          ;;
        esac
    ])
AC_MSG_RESULT([$BUILD_SYS])
fi
AC_SUBST([MODEXT])
AC_DEFINE_UNQUOTED([MODULE_EXT], "$MODEXT", [Extensión para módulos en tiempo real])
AC_SUBST([BUILD_SYS])
AC_SUBST([EXT_RTFLAGS])

###########################################################################################
# Subsección 3.3 - comprobar el directorio donde pueden instalarse los módulos            # 
#                                                                                         # 
# Proporciona al usuario la posibilidad de especificarlo (--with-module-dir=<path>)       # 
# Si no se especifica, por defecto en /lib/modules/${KERNEL_VERS}/                        # 
# a la ubicación donde se encuentra un módulo RT (mbuff.o o rtai_shed.o)                  # 
# (¿quizás el valor predeterminado debería ser /lib/modules/$KERNEL_VERS/extra si existe? # 
###########################################################################################

# Crear la opcion module-dir, similar a rtai's...
AC_MSG_CHECKING([para directorio de instalacion de modulos])

AC_ARG_WITH(module-dir,
    AS_HELP_STRING(
        [--with-module-dir=PATH],
        [Directorio de instalacion de modulos de kernel.]
    ),
    [
        case "$withval" in
        "" | y | ye | yes | n | no)
            AC_MSG_ERROR([Debe dar un path para --with-module-dir.])
          ;;
        esac
        MODULE_DIR="$withval"
    ])


# si no está especificado, pregunta al script de configuración de tiempo real donde está el directorio
# si el script de configuración devuelve un error, establece MODULE_DIR a nada
if test "x" = "x$MODULE_DIR" ; then
  MODULE_DIR=`$RTS --module-dir 2>/dev/null`
  if ! test -d $MODULE_DIR ; then
    # imprimir un mensaje de error en la salida de error estándar y salir.
    AC_MSG_ERROR([$RTS --module-dir no especificó un directorio existente. Debe especificarlo usando --with-module-dir=...])
  fi
fi
    
if test "$RUN_IN_PLACE" = "yes"; then
    RIP_MODULE_DIR="${BUILD_TOPLEVEL}/rtlib"
    AC_MSG_RESULT([configurando para run-in-place])
    prefix=${BUILD_TOPLEVEL}
    DEFAULT_NMLFILE=${prefix}/configs/common/linuxcnc.nml
    DEFAULT_TOOLTABLE=${prefix}/configs/sim/sim.tbl
    AC_MSG_RESULT([$RIP_MODULE_DIR])
else
    RUN_IN_PLACE=no
    EMC2_HOME=$prefix
    DEFAULT_NMLFILE=${prefix}/share/linuxcnc/linuxcnc.nml
    AC_MSG_RESULT([$MODULE_DIR])
fi

AC_SUBST([MODULE_DIR])
AC_SUBST([RIP_MODULE_DIR])
AC_SUBST([RUN_IN_PLACE])
AC_SUBST([DEFAULT_NMLFILE])
AC_DEFINE_UNQUOTED([EMC2_DEFAULT_NMLFILE], "$DEFAULT_NMLFILE", [Default nml file])
AC_DEFINE_UNQUOTED([EMC2_DEFAULT_TOOLTABLE], "$DEFAULT_TOOLTABLE", [Default nml file])

#########################################################################################
# Subsection 3.4 - definir directorios predeterminados, que se usan en                  # 
# muchos lugares. Las decisiones se basan en si se instala como RIP o instalacion normal#
#########################################################################################

fe () {
    # Expandir completamente argumento. Ejemplo:
    # ${datadir} -> ${datarootdir} -> ${prefix}/share -> /usr/local
    case "$1" in
    *\$*) eval fe \""$1"\" ;;
    *) echo "$1" ;;
    esac
}

if test "xyes" = "x$RUN_IN_PLACE"; then
    EMC2_LATENCY_SCRIPT=$EMC2_HOME/scripts/latency-test
    EMC2_SCRIPT=$EMC2_HOME/scripts/linuxcnc
    EMC2_SUFFIX=" (run-in-place)"
    EMC2_ICON=$EMC2_HOME/linuxcncicon.png
    EMC2_BIN_DIR=$EMC2_HOME/bin
    EMC2_TCL_DIR=$EMC2_HOME/tcl
    EMC2_TCL_LIB_DIR=$EMC2_HOME/tcl
    EMC2_LANG_DIR=$EMC2_HOME/src/objects
    EMC2_PO_DIR=$EMC2_HOME/share/locale
    EMC2_HELP_DIR=$EMC2_HOME/docs/help
    EMC2_RTLIB_DIR=$EMC2_HOME/rtlib
    LINUXCNC_CONFIG_PATH="~/linuxcnc/configs:$EMC2_HOME/configs"
    EMC2_NCFILES_DIR=$EMC2_HOME/nc_files
    REALTIME=$EMC2_HOME/scripts/realtime
    EMC2_IMAGE_DIR=$EMC2_HOME
    HALLIB_DIR=$EMC2_HOME/lib/hallib
else
    EMC2_HOME=${prefix}
    EMC2_BIN_DIR=${prefix}/bin
    EMC2_LATENCY_SCRIPT=$EMC2_BIN_DIR/latency-test
    EMC2_SCRIPT=$EMC2_BIN_DIR/linuxcnc
    EMC2_SUFFIX=""
    EMC2_ICON=linuxcncicon
    EMC2_TCL_DIR=${prefix}/lib/tcltk/linuxcnc
    EMC2_TCL_LIB_DIR=${prefix}/lib/tcltk/linuxcnc
    EMC2_LANG_DIR=${prefix}/lib/tcltk/linuxcnc/msgs
    EMC2_PO_DIR=${prefix}/share/locale
    EMC2_HELP_DIR=${prefix}/share/doc/linuxcnc
    case $MODULE_DIR in
        */linuxcnc*) EMC2_RTLIB_DIR=$MODULE_DIR ;;
	*) EMC2_RTLIB_DIR=$MODULE_DIR/linuxcnc
    esac
    LINUXCNC_CONFIG_PATH="~/linuxcnc/configs:/usr/local/etc/linuxcnc/configs:"$(eval echo $EMC2_HELP_DIR)"/examples/sample-configs"
    EMC2_NCFILES_DIR=${prefix}/share/linuxcnc/ncfiles
    REALTIME=/etc/init.d/realtime
    EMC2_IMAGE_DIR=$(fe "$datadir")/linuxcnc
    HALLIB_DIR=${prefix}/share/linuxcnc/hallib
fi

AC_DEFINE_UNQUOTED([EMC2_HOME], "$EMC2_HOME", [Prefix o directorio RIP])
AC_DEFINE_UNQUOTED([EMC2_BIN_DIR], "$EMC2_BIN_DIR", [Directorio para binarios])
AC_DEFINE_UNQUOTED([EMC2_TCL_DIR], "$EMC2_TCL_DIR", [Directorio para scripts tcl])
AC_DEFINE_UNQUOTED([EMC2_HELP_DIR], "$EMC2_HELP_DIR", [Directorio para archivos de ayuda])
AC_DEFINE_UNQUOTED([EMC2_RTLIB_DIR], "$EMC2_RTLIB_DIR", [Directorio del sistema realtime])
AC_DEFINE_UNQUOTED([EMC2_LANG_DIR], "$EMC2_LANG_DIR", [Directorio para archivos de traduccion tcl])
AC_DEFINE_UNQUOTED([EMC2_PO_DIR], "$EMC2_PO_DIR", [Directorio para archivos de traduccion po/mo])
AC_DEFINE_UNQUOTED([EMC2_CONFIG_DIR], "$EMC2_CONFIG_DIR", [Directorio para archivos de configuracion])
AC_DEFINE_UNQUOTED([EMC2_NCFILES_DIR], "$EMC2_NCFILES_DIR", [Directorio para archivos nc])
AC_DEFINE_UNQUOTED([EMC2_IMAGE_DIR], "$EMC2_IMAGE_DIR", [Directorio para imagenes])

AC_SUBST([EMC2_BIN_DIR])
AC_SUBST([EMC2_TCL_DIR])
AC_SUBST([EMC2_TCL_LIB_DIR])
AC_SUBST([EMC2_HELP_DIR])
AC_SUBST([EMC2_RTLIB_DIR])
AC_SUBST([EMC2_LANG_DIR])
AC_SUBST([LINUXCNC_CONFIG_PATH])
AC_SUBST([EMC2_NCFILES_DIR])
AC_SUBST([REALTIME])
AC_SUBST([EMC2_IMAGE_DIR])
AC_SUBST([EMC2_SCRIPT])
AC_SUBST([EMC2_LATENCY_SCRIPT])
AC_SUBST([EMC2_SUFFIX])
AC_SUBST([EMC2_ICON])
AC_SUBST([HALLIB_DIR])

# coordinate installed auxiliary applications (not RIP or deb dependent):
LINUXCNC_AUX_GLADEVCP=/usr/share/linuxcnc/aux_gladevcp
LINUXCNC_AUX_EXAMPLES=/usr/share/linuxcnc/aux_examples
AC_SUBST([LINUXCNC_AUX_GLADEVCP])
AC_SUBST([LINUXCNC_AUX_EXAMPLES])

##############################################################################
# Subsection 3.5 - check para GTK                                            #
# FIXME: permitir habilitacion o no en linea de comandos                     #
##############################################################################

AC_MSG_CHECKING([for glib])
if pkg-config glib-2.0 >/dev/null 2>&1; then
  GLIB_VER=`pkg-config glib-2.0 --modversion`
  AC_MSG_RESULT(yes - $GLIB_VER)
  GLIB_CFLAGS=`pkg-config glib-2.0 --cflags`
  GLIB_LIBS=`pkg-config glib-2.0 --libs`
else
  AC_MSG_ERROR(no -- required until somebody makes glib optional)
fi


AC_ARG_ENABLE(gtk,
    AS_HELP_STRING(
        [--disable-gtk],
        [Deshabilita las partes de LinuxCNC que dependen de GTK.]
    ),
    [
	case "$enableval" in
	Y*|y*)
	    BUILD_GTK=yes ;;
	*)
	    BUILD_GTK=no ;;
	esac
    ],
    [BUILD_GTK=yes])

if test "$BUILD_GTK" = "yes"; then
  AC_MSG_CHECKING([para GTK 2.4.0 o superior])
  if pkg-config gtk+-2.0 --atleast-version 2.4.0 >/dev/null 2>&1; then
    GTK_VER=`pkg-config gtk+-2.0 --modversion`
    AC_MSG_RESULT(yes - $GTK_VER)
    GTK_CFLAGS=`pkg-config gtk+-2.0 --cflags`
    GTK_LIBS=`pkg-config gtk+-2.0 --libs`
    AC_MSG_CHECKING(para libgnomeprintui-2.2)
    if pkg-config --exists libgnomeprintui-2.2; then
      AC_MSG_RESULT(yes)
      GTK_CFLAGS="$GTK_CFLAGS `pkg-config --cflags libgnomeprintui-2.2`"
      GTK_LIBS="$GTK_LIBS `pkg-config --libs libgnomeprintui-2.2`"
      HAVE_GNOMEPRINT=yes
    else
      AC_MSG_RESULT(no -- No será posible imprimir desde classicladder)
      HAVE_GNOMEPRINT= 
    fi
  else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([Falta GTK2. Instálelo o especifique --disable-gtk para omitir las partes de LinuxCNC que dependen de GTK])
  fi
fi

AC_SUBST(HAVE_GNOMEPRINT)
AC_SUBST(GTK_VER)
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_CFLAGS)

##############################################################################
# Subsection 3.6 - comprobar programas de utilidad necesarios para compilar, #
# instalar y ejecutar linuxcnc, así como los usados en los scripts           # 
##############################################################################

SPATH=$PATH:/usr/local/sbin:/usr/sbin:/sbin
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PATH_PROG(AR,ar)
AC_PATH_PROG(INSTALL, install)

AC_PATH_PROG(SED, sed, "none")
if test $SED = "none"
then
    AC_MSG_ERROR([sed no encontrado])
fi

AC_PATH_PROG(PS, ps, "none")
if test $PS = "none"
then
    AC_MSG_ERROR([ps no encontrado])
fi

AC_PATH_PROG(KILL, kill, "none")
if test $KILL = "none"
then
    AC_MSG_ERROR([kill no encontrado])
fi

AC_PATH_PROG(WHOAMI, whoami, "none")
if test $WHOAMI = "none"
then
    AC_MSG_ERROR([whoami no encontrado])
fi

AC_PATH_PROG(AWK, awk, "none")
if test $AWK = "none"
then
    AC_MSG_ERROR([awk no encontrado])
fi

AC_PATH_PROG(INSMOD, insmod, "none", $SPATH)
if test $INSMOD = "none"
then
    AC_MSG_ERROR([insmod no encontrado])
fi

AC_PATH_PROG(RMMOD, rmmod, "none", $SPATH)
if test $RMMOD = "none"
then
    AC_MSG_ERROR([rmmod no encontrado])
fi

AC_PATH_PROG(LSMOD, lsmod, "none", $SPATH)
if test $LSMOD = "none"
then
    AC_MSG_ERROR([lsmod no encontrado])
fi

AC_PATH_PROG(PIDOF, pidof, "none", $SPATH)
if test $PIDOF = "none"
then
    AC_MSG_ERROR([pidof no encontrado])
fi

AC_PATH_PROG(IPCS, ipcs, "none")
if test $IPCS = "none"
then
    AC_MSG_ERROR([ipcs no encontrado])
fi

AC_PATH_PROG(FUSER, fuser, "none", $SPATH)
if test $FUSER = "none"
then
    AC_MSG_ERROR([fuser no encontrado])
fi

AC_PATH_PROGS(YAPPS, yapps yapps2, "none", $SPATH)
if test $YAPPS = "none"
then
    AC_MSG_ERROR([yapps/yapps2 no encontrado])
fi
AC_SUBST(YAPPS)

# MANDB empty is handled in doc/Submakefile
AC_PATH_PROG(MANDB, mandb, "")

AC_PATH_PROG(INTLTOOL_EXTRACT, intltool-extract, "none", $SPATH)
if test $INTLTOOL_EXTRACT = "none"
then
    AC_MSG_ERROR([intltool-extract no encontrado])
fi

AC_ARG_WITH(rmmod,
    AS_HELP_STRING(
        [--with-rmmod=PATH],
        [Localizacion del programa rmmod a usar.]
    ),
    [
	case "$withval" in
	"" | y | ye | yes)
	    AC_MSG_ERROR([Debe dar un argumento a --with-rmmod.])
	  ;;
	n | no | none)
	    RMMOD=""
	  ;;
	*)
	    RMMOD="$withval"
	esac
    ],
    [
            RMMOD="$EMC2_BIN_DIR/linuxcnc_module_helper remove"
    ])

AC_ARG_WITH(insmod,
    AS_HELP_STRING(
        [--with-insmod=PATH],
        [Localizacion del programa insmod a usar.]
    ),
    [
	case "$withval" in
	"" | y | ye | yes)
	    AC_MSG_ERROR([Debe dar un argumento a --with-insmod.])
	  ;;
	n | no | none)
	    INSMOD=""
	  ;;
	*)
	    INSMOD="$withval"
	esac
    ],
    [
            INSMOD="$EMC2_BIN_DIR/linuxcnc_module_helper insert"
    ])

AC_PATH_PROGS(YAPPS, [yapps yapps2])
if test -z "$YAPPS"; then
  AC_MSG_ERROR([se requiere yapps para construir LinuxCNC
instalelo con "sudo apt-get install yapps2"])
fi
AC_SUBST(YAPPS)

##############################################################################
# Subsection 3.7 - chequear programas necesarios para compilar documentación # 
#                                                                            # 
# Comprueba LyX y programas necesarios para compilar/instalar documentos.    # 
# (Opcional. Si no está presente, simplemente no compilar la documentacion). # 
##############################################################################

AC_ARG_WITH(python,
    AS_HELP_STRING(
        [--with-python=PATH],
        [Especifica interprete Python.]
    ),
    [
        PYTHON=$withval
    ],[
        PYTHON=python
    ])
AC_PATH_PROG(PYTHON,$PYTHON,"none")
if test $PYTHON = "none"
then
    AC_MSG_ERROR([python no encontrado])
fi

AC_MSG_CHECKING([si compilar documentación])
AC_ARG_ENABLE(build-documentation,
    AS_HELP_STRING(
        [--enable-build-documentation[=FORMAT]],
        [Compilar documentación. FORMAT puede ser "pdf" o "html" si solo se desea un formato.]
    ),
    [
        case "$enableval" in
        Y*|y*|html,pdf|pdf,html)
            BUILD_DOCS=yes
            BUILD_DOCS_PDF=yes
            BUILD_DOCS_HTML=yes
            AC_MSG_RESULT([Solicitado PDF y HTML])
        ;;
        html)
            BUILD_DOCS=yes
            BUILD_DOCS_PDF=no
            BUILD_DOCS_HTML=yes
            AC_MSG_RESULT([HTML requested])
	;;
        pdf)
            BUILD_DOCS=yes
            BUILD_DOCS_PDF=yes
            BUILD_DOCS_HTML=no
            AC_MSG_RESULT([PDF requested])
	;;
        *)
            BUILD_DOCS=no
            AC_MSG_RESULT([no])
        ;;
    esac
    ],
    [
        BUILD_DOCS=no
        AC_MSG_RESULT([no])
    ])
# Programas requeridos para construir toda la documentación.
if ( test "$BUILD_DOCS" = "yes" ) ; then
    if ( test "none" = "$PYTHON" ) ; then
        AC_MSG_ERROR([no hay python, no se puede construir documentación])
    fi

    AC_PATH_PROG(ASCIIDOC,asciidoc,"none")
    if ( test "none" = "$ASCIIDOC" ) ; then
	AC_MSG_ERROR([no hay AsciiDoc, no se puede construir documentación])
    fi

    AC_PATH_PROG(A2X,a2x,"none")
    if ( test "none" = "$A2X" ) ; then
	AC_MSG_ERROR([no hay a2x, no se puede construir documentación])
    fi

    AC_MSG_CHECKING([si se debe especificar latex.encoding])
    temp_asciidoc=`tempfile --suffix=.txt`
    cat > $temp_asciidoc <<EOF
:lang: fr
:ascii-ids:

....
umläut nuß
....
EOF
    if $A2X --dblatex-opts "-P latex.encoding=utf8" $temp_asciidoc > /dev/null 2>&1;
    then
        A2X_LATEX_ENCODING="-P latex.encoding=utf8"
        AC_MSG_RESULT(yes)
    else
        A2X_LATEX_ENCODING=""
        AC_MSG_RESULT(no)
    fi
    AC_SUBST(A2X_LATEX_ENCODING)
    rm -f $temp_asciidoc ${temp_asciidoc%.txt}.pdf

    AC_PATH_PROG(DBLATEX,dblatex,"none")
    if ( test "none" = "$DBLATEX" ) ; then
	AC_MSG_ERROR([no hay dblatex, no se puede construir documentación])
    fi

    AC_MSG_CHECKING([dblatex version])
    set -- `dblatex --version`; DBLATEX_VER=$3
    set -- `echo $DBLATEX_VER | sed 's/[[.-]]/ /g'`

    if test $1 -eq 0 -a \( $2 -lt 2 -o \( $2 -eq 2 -a ${3:-0} -lt 12 \) \); then
        AC_MSG_ERROR([version dblatex $DBLATEX_VER menor que 0.2.12.
No se puede construir documentación.])
    fi
    AC_MSG_RESULT([$DBLATEX_VER])


    AC_PATH_PROG(SOURCE_HIGHLIGHT,source-highlight,"none")
    if ( test "none" = "$SOURCE_HIGHLIGHT" ) ; then
	AC_MSG_ERROR([no hay convert, no se puede construir documentación])
    fi


    AC_PATH_PROG(CONVERT,convert,"none")
    if ( test "none" = "$CONVERT" ) ; then
	AC_MSG_ERROR([no convert, documentation cannot be built])
    fi

    AC_PATH_PROG(GS,gs,"none")
    if ( test "none" = "$GS" ) ; then
	AC_MSG_ERROR([no hay gs, no se puede construir documentación])
    fi
fi

# Programas requeridos para documentacion PDF
if ( test "$BUILD_DOCS_PDF" = "yes" ) ; then
    AC_PATH_PROG(DBLATEX,dblatex,"none")
    if ( test "none" = "$DBLATEX" ) ; then
	AC_MSG_ERROR([no hay dblatex, no se puede construir documentación PDF])
    fi

    AC_PATH_PROG(PDFLATEX,pdflatex,"none")
    if ( test "none" = "$PDFLATEX" ) ; then
	AC_MSG_ERROR([no hay pdflatex, no se puede construir documentación PDF])
    fi
fi

# Programas requeridos para documentacion HTML
if ( test "$BUILD_DOCS_HTML" = "yes" ) ; then
    AC_PATH_PROG(XSLTPROC,xsltproc,"none")
    if ( test "none" = "$XSLTPROC" ) ; then
	AC_MSG_ERROR([no hay xsltproc, no se puede construir documentación HTML])
    fi

    AC_PATH_PROG(DVIPNG,dvipng,"none")
    if ( test "none" = "$DVIPNG" ) ; then
	AC_MSG_ERROR([no hay dvipng, no se puede construir documentación HTML])
    fi

    AC_MSG_CHECKING([for HTML support in groff])
    if ! groff -Thtml < /dev/null > /dev/null 2>&1 ; then
        AC_MSG_ERROR([no hay groff -Thtml, no se puede construir documentación HTML])
    else
        AC_MSG_RESULT(yes)
    fi

    AC_PATH_PROG(CHECKLINK,checklink,"none")
    if ( test "none" = "$CHECKLINK" ) ; then
	AC_MSG_ERROR([no hay checklink, no se puede construir documentación HTML
instalar con "sudo apt-get install w3c-linkchecker"])
    fi
fi
AC_SUBST(BUILD_DOCS)
AC_SUBST(BUILD_DOCS_PDF)
AC_SUBST(BUILD_DOCS_HTML)

if test "$RTS" '!=' uspace
then
for m in adeos rtai_hal rtai_ksched rtai_sched rtai_fifos rtai_sem rtai_math \
; do
    AC_MSG_CHECKING([for $m])
    TMP=$(find $MODULE_DIR -name $m$MODEXT 2>/dev/null)
    if test -z "$TMP"; then
        TMP=$(find /lib/modules/$KERNEL_VERS -name $m$MODEXT)
    fi
    eval MODPATH_$m=${TMP/$KERNEL_VERS/\\\$\\\(uname\\\ -r\\\)}
    AC_MSG_RESULT([${TMP:-no encontrado}])
done

AC_SUBST(MODPATH_adeos)
AC_SUBST(MODPATH_rtai_hal)
AC_SUBST(MODPATH_rtai_ksched)
AC_SUBST(MODPATH_rtai_sched)
AC_SUBST(MODPATH_rtai_fifos)
AC_SUBST(MODPATH_rtai_sem)
AC_SUBST(MODPATH_rtai_math)
fi

# Checks para headers.

##############################################################################
# Section 4.1 - Cabeceras, funciones y definiciones globales importantes     #
#                                                                            #
##############################################################################

# Esta sección se inserta en config.h.in cuando se ejecuta autoheader. 
# Las definiciones globales y comunes deben estar aquí en lugar de duplicarse en 
# múltiples lugares. Mantenga las construcciones mas "complicadas" fuera de aqui, y podran 
# ser utilizadas tanto en el espacio del kernel como en el espacio del usuario. 
AH_TOP([/********************************************************************
* Descripcion: config.h
*
* 	Definiciones comunes utilizadas en muchos archivos fuente de LinuxCNC.
* 	Para agregar a este archivo, edite la Sección 4.1 en configure.ac
* 	y luego ejecute autoheader.
*
* Author: Autogenerated by autoheader
* License: LGPL Version 2
* System: Linux
*    
* Copyright (c) 2004 All rights reserved.
********************************************************************/
#ifndef EMC2_CONFIG_H
#define EMC2_CONFIG_H

/* LINELEN se usa en todos los tamaños de búfer, longitud de las cadenas de nombre de archivo, etc.
    Solo tenemos uno en lugar de una multitud de define iguales. */
#define LINELEN 255
/* Usado en varios lugares para buffers sprintf(). */
#define BUFFERLEN 80

#define MM_PER_INCH 25.4
#define INCH_PER_MM (1.0/25.4)
])

AC_CHECK_HEADERS([sys/io.h])

AH_VERBATIM([_GNU_SOURCE],
[/* Habilitar las extensiones GNU en los sistemas que las tienen.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
])

AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_FUNCS(semtimedop)
LIBS_hold=$LIBS
LIBS="$LIBS -lm"

AC_MSG_CHECKING([para funcion sincos])
sincos_ok=yes
AC_TRY_LINK(
    [double a, b;],
    [sincos(1.0, &a, &b);],
    AC_DEFINE(
        [HAVE_SINCOS],
        1,
        [Define a 1 si sincos esta disponible.]
    ),
    [sincos_ok=no]
)
AC_MSG_RESULT([$sincos_ok])

AC_CHECK_FUNCS(__sincos)
LIBS=$LIBS_hold

AC_MSG_CHECKING([para optreset])
AC_TRY_LINK(
    [
#include <unistd.h>
extern int optreset;
    ],
    [ optreset = 1; ],
    [
	HAVE_OPTRESET=yes
        AC_DEFINE([HAVE_OPTRESET], [], [Define a 1 si getopt tiene la extension BSD 'optreset'])
],
    [HAVE_OPTRESET=no])
AC_MSG_RESULT($HAVE_OPTRESET)
AC_SUBST([HAVE_OPTRESET])



LIBS_hold=$LIBS
LIBS=
AC_SEARCH_LIBS(dlopen, [c dl], [
    case "$LIBS" in
    (-lc) LIBDL= ;;
    (*)   LIBDL="$LIBS"
    esac

    AC_DEFINE([LIBDL], [$LIBDL], [Define a la biblioteca de la que proviene dlopen, si no es parte de libc.])
], [AC_MSG_ERROR([no encontrado])])
AC_SUBST([LIBDL])
LIBS=$LIBS_hold

AC_SEARCH_LIBS(clock_nanosleep, [rt], [
    AC_DEFINE([HAVE_CLOCK_NANOSLEEP], 1,
        [Define a 1 si clock_nanosleep esta disponible (en -lc o -lrt)])
])

AH_BOTTOM(#endif
)

##############################################################################
# Section 5 - Verificar soporte tcl/tk                                       # 
#                                                                            # 
# El test para los encabezados tcl/tk falla si no están en /usr/include      # 
# por lo que los tests se realizan al encontrar tclConfig.sh y tkConfig.sh   # 
# si la búsqueda falla, se realiza el chequeo tradicional                    # 
##############################################################################
AC_MSG_CHECKING([para tcl])

AC_ARG_WITH(tclConfig,
    AS_HELP_STRING(
        [--with-tclConfig=PATH],
        [Especifica el path a tclConfig.]
    ),
    [
        TCLCONFIG=$withval
    ],[
        TCLCONFIG=
    ])

# si no especificado, buscar tclConfig.sh
if (test "x$TCLCONFIG" = "x"); then
    TCLCONFIG=$(find /usr/lib* -maxdepth 2 -name tclConfig.sh | sort -r | head -n1)
    if (test "x$TCLCONFIG" = "x"); then
	TCLCONFIG=$(find /usr/local/lib* -maxdepth 2 -name tclConfig.sh | sort -r | head -n1)
    fi
fi

if (test "x$TCLCONFIG" = "x"); then
   AC_MSG_RESULT([tclConfig.sh no encontrado, intentando tcl.h y libs.
		  Si no funciona, intente correr ./configure --with-tclConfig=<path to tclConfig.sh>])
   AC_CHECK_HEADERS([tcl.h])
   AC_SEARCH_LIBS(Tcl_Init,[tcl tcl8.1 tcl8.2 tcl8.3 tcl8.4 tcl8.5],[],[AC_MSG_ERROR([libreria tcl no encontrada])])
else
# tclConfig.sh encontrado,
# extrayendo vars
   AC_MSG_RESULT([$TCLCONFIG encontrado])
   . "$TCLCONFIG"
   TCL_CFLAGS="$TCL_INCLUDE_SPEC -DUSE_TCL_STUBS"
   TCL_LIBS="$TCL_STUB_LIB_SPEC"
fi

AC_MSG_CHECKING([para tk])

AC_ARG_WITH(tkConfig,
    AS_HELP_STRING(
        [--with-tkConfig=PATH],
        [Especifica el path a tkConfig.]
    ),
    [
        TKCONFIG=$withval
    ],[
        TKCONFIG=
    ])

# if not specified, search for tclConfig.sh
if test -z "$TKCONFIG"; then
    TKCONFIG=$(dirname $TCLCONFIG)/tkConfig.sh
    if ! test -f $TKCONFIG; then unset TKCONFIG; fi
fi
if (test "x$TKCONFIG" = "x"); then
    # busqueda de tkConfig.sh
    TKCONFIG=$(find /usr/lib -maxdepth 2 -name tkConfig.sh | sort -r | head -n1)
    if (test "x$TKCONFIG" = "x"); then
	TKCONFIG=$(find /usr/local/lib -maxdepth 2 -name tkConfig.sh | head -n1)
    fi
fi

if (test "x$TKCONFIG" = "x"); then
   AC_MSG_RESULT([tkConfig.sh no encontrado, intentando tk.h and libs.
		  Si no funciona, intente correr ./configure --with-tkConfig=<path to tkConfig.sh>])
   AC_CHECK_HEADERS([tk.h])
   AC_SEARCH_LIBS(Tk_Init,[tk tk80 tk8.0 tk8.1 tk8.2 tk8.3 tk8.4 tk8.5],[],[AC_MSG_ERROR([libreria tk no encontrada])])
else
# tkConfig.sh encontrado,
# extracting vars
   AC_MSG_RESULT([$TKCONFIG encontrado])
   . "$TKCONFIG"
   TK_CFLAGS="$TK_INCLUDE_SPEC $TK_XINCLUDES" 
   TK_LIBS="$TK_LIBS $TK_LIB_SPEC"
fi

if ! test -z "$TCL_VERSION" && ! test -z "$TK_VERSION" && ! test "$TCL_VERSION" = "$TK_VERSION" ; then
    AC_MSG_ERROR([Las versiones Tcl y Tk deben ser las mismas, pero configure ha encontrado
Tcl $TCL_VERSION y Tk $TK_VERSION. Puede usar --with-tkConfig= y --with-tclConfig=
para ajustar las versiones autodetectadas.])
fi

if test -f $TCL_EXEC_PREFIX/bin/wish$TCL_VERSION; then
    WISH=$TCL_EXEC_PREFIX/bin/wish$TCL_VERSION
elif test -f $TCL_EXEC_PREFIX/bin/wish; then
    WISH=$TCL_EXEC_PREFIX/bin/wish
else
    WISH=wish
fi

if test -f $TCL_EXEC_PREFIX/bin/tclsh$TCL_VERSION; then
    TCLSH=$TCL_EXEC_PREFIX/bin/tclsh$TCL_VERSION
elif test -f $TCL_EXEC_PREFIX/bin/tclsh; then
    TCLSH=$TCL_EXEC_PREFIX/bin/tclsh
else
    TCLSH=tclsh
fi

##############################################################################
# Sección 6.- 
# Si se da el parámetro --disable-check-runtime-deps en ./configure, se omitirán ciertas comprobaciones de requisitos de tiempo de
# ejecución no necesarios durante el proceso de construcción; en particular: 
# BWidget 
# wish 
# img::png 
# python Pango y Cairo
##############################################################################

AC_ARG_ENABLE(check-runtime-deps,
    AS_HELP_STRING(
        [--disable-check-runtime-deps],
        [Omitir chequeo de los requisitos de tiempo de ejecución no necesarios durante el
         proceso de compilacion.]
    ),
    [RUNTIME_CHECK=$enableval],
    [RUNTIME_CHECK=yes])
AC_MSG_CHECKING([Comprobar si hay dependencias de tiempo de ejecución.])
AC_MSG_RESULT($RUNTIME_CHECK)

if test "$RUNTIME_CHECK" = "yes"; then
AC_MSG_CHECKING([para BWidget usando $TCLSH])
if (unset DISPLAY; echo ["catch { el paquete requiere bwidget }; exit [expr [lsearch [package names] BWidget] == -1]"] | $TCLSH); then
    AC_MSG_RESULT([encontrado])
else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([BWidget no encontrado!
instalar con "sudo apt-get install bwidget"])
fi

AC_MSG_CHECKING([para BLT usando tclsh8.5])
if (test -f $TCL_EXEC_PREFIX/bin/tclsh8.5) && (echo ['if [catch {el paquete requiere BLT}] { exit 1; } else { exit 0; }'] | tclsh8.5); then
    AC_MSG_RESULT([encontrado])
else
    AC_MSG_RESULT([no encontrado])
fi

AC_MSG_CHECKING([para img::png usando $TCLSH])
if (unset DISPLAY; echo ["catch { el paquete requiere img::png }; exit [expr [lsearch [package names] img::png] == -1]"] | $TCLSH); then
    AC_MSG_RESULT([encontrado])
    HAVE_WORKING_BLT=yes
else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([Tk img::png no encontrado!
instalar con "sudo apt-get install libtk-img"])
fi

AC_MSG_CHECKING([para tclX usando $TCLSH])
if (unset DISPLAY; echo ["catch { package require Tclx }; exit [expr [lsearch [package names] Tclx] == -1]"] | $TCLSH); then
    AC_MSG_RESULT([found])
else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([Tclx no encontrado!
instalar con "sudo apt-get install tclx"])
fi

AC_MSG_CHECKING([para modulos python pango y cairo])
if $PYTHON -c 'import sys;raise SystemExit(sys.hexversion<0x3040000)'
then

	if $PYTHON -c "import gi;gi.require_version('Pango', '1.0');from gi.repository import Pango"; then
	    AC_MSG_RESULT([found])
	else
	    AC_MSG_RESULT(no)
	    AC_MSG_ERROR([modulos python pango y cairo modules no encontrados!\ninstalar con "sudo apt-get install python3-gi"])
	fi

else

	if $PYTHON -c 'import pango,cairo'; then
	    AC_MSG_RESULT([encontrado])
	else
	    AC_MSG_RESULT(no)
	    AC_MSG_ERROR([modulos python pango y cairo no encontrados!\ninstalar con "sudo apt-get install python-gtk2"])
	fi
	fi
fi

AC_SUBST([TCL_DBGX])
AC_SUBST([TK_DBGX])
AC_SUBST([TCL_CFLAGS])
AC_SUBST([TK_CFLAGS])
AC_SUBST([TCL_LIBS])
AC_SUBST([TK_LIBS])
AC_SUBST([WISH])
AC_SUBST([TCLSH])

##############################################################################
# Sección 7 - Otros Soportes                                                 # 
#                                                                            # 
# Subsección 7.1. - proporciona DATE para enviarlo a los archivos generados  # 
# Subsección 7.2. - Opción de usuario para silenciar make                    #
##############################################################################

# proporciona DATE para enviarlo a los archivos generados
DATE=$(date)
AC_SUBST([DATE])
AC_PATH_XTRA
#X_CFLAGS gets set
#X_LIBS and X_EXTRA_LIBS likewise

AC_SUBST([X_LIBS])
AC_SUBST([X_CFLAGS])

AC_CHECK_HEADERS(X11/extensions/Xinerama.h,
	    [HAVE_XINERAMA=yes],
	    [HAVE_XINERAMA=no])

AC_CHECK_LIB(Xinerama, XineramaQueryExtension,
	    [],
	    [HAVE_XINERAMA=no])

if test "x$HAVE_XINERAMA" = "xno"; then
    AC_MSG_ERROR([Cabeceras o libreria Xinerama no encontrada])
fi

#Limpiar LIBS
temp_LIBS=$LIBS
LIBS=

# chequeo para readline.h y asociados, opcional para halcmd
AC_CHECK_HEADERS(readline/readline.h, [HAVE_READLINE=yes], [HAVE_READLINE=no])
AC_CHECK_HEADERS(readline/history.h, [], [HAVE_READLINE=no])

echo "Intentando readline sin -ltermcap"
AC_CHECK_LIB(readline, readline, [HAVE_READLINE_NOTERMCAP=yes], [])
unset ac_cv_lib_readline_readline
echo "Intentando readline con -ltermcap"
AC_CHECK_LIB(readline, readline, [HAVE_READLINE_TERMCAP=yes], [], -ltermcap)
unset ac_cv_lib_readline_readline

if test "x$HAVE_READLINE_NOTERMCAP" = "xyes"; then
    echo "Readline no requiere -ltermcap"
    READLINE_LIBS="-lreadline"
elif test "x$HAVE_READLINE_TERMCAP" = "xyes"; then
    echo "Readline requiere -ltermcap"
    READLINE_LIBS="-lreadline -ltermcap"
else
    HAVE_READLINE=no
fi

AC_MSG_CHECKING([si licencia readline es compatible con GPL-2])
AC_TRY_COMPILE([
        #include <readline/readline.h>
    ],[
        #if RL_VERSION_MAJOR > 5
        #error Readline version 6 and up are not compatible with GPL-2
        #endif
    ],[AC_MSG_RESULT(yes)],[
        AC_MSG_RESULT(no)
        AC_MSG_WARN(
[El binario de LinuxCNC que está compilando puede no ser
distribuible debido a una incompatibilidad de licencia con LinuxCNC (algunas partes
son GPL-2 solo) y Readline versión 6 y superior (GPL-3 o posterior).])
        if $NONDISTRIBUTABLE; then
            AC_MSG_WARN(
[Se requiere del usuario habilitar compilacion non-distribuible.  Continuando.])
        else
            AC_MSG_ERROR(
[Para configurar LinuxCNC de esta manera, debe
invocar configure con "--enable-non-distributable=yes". Tenga en cuenta que en
los sistemas basados en Debian, puedes usar libreadline-gplv2-dev en su lugar.])
        fi
])

if test "x$HAVE_READLINE" = "xyes"; then
    AC_DEFINE([HAVE_READLINE], [], [Define a 1 si tiene la biblioteca 'readline' (-lreadline) y los encabezados requeridos])
else
    AC_MSG_ERROR([Libreria requerida 'readline' no encontrada.])
fi
AC_SUBST([HAVE_READLINE])
AC_SUBST([READLINE_LIBS])

##############################################################################
# Section 8 - soporte de Lenguages                                           # 
#                                                                            # 
# soporte i18n:                                                              # 
# ver http://cvs.gnome.org/viewcvs/intltool/doc/I18N-HOWTO?rev=1.3           # 
##############################################################################

# Si no tiene las herramientas nls, ¡deja una salida!

AC_ARG_ENABLE(nls,
    AS_HELP_STRING([--disable-nls], [No usar NLS.]),
              USE_NLS=no, USE_NLS=yes)
AC_SUBST(USE_NLS)

if test "$USE_NLS" = "yes"; then
  AC_CHECK_HEADERS(locale.h)
  AC_CHECK_FUNCS(setlocale)
  AC_SUBST(HAVE_LOCALE_H)  

# Agregar idiomas aquí cuando están traducidos.
# Grep el archivo LINGUAS.
  LANGUAGES=""
  AC_SUBST(LANGUAGES)  
  AC_CHECK_HEADERS(libintl.h)
  AC_CHECK_LIB(intl,gettext)
  AC_SUBST(HAVE_LIBINTL_H)  
  AC_SUBST(HAVE_LIBINTL)

# El directorio de configuración regional predeterminado es /usr/share/locale - Esto generalmente está dispuesto
# por la distribución ....
  dnl Handle localedir
  LOCALEDIR='/usr/share/locale'
  AC_ARG_WITH(locale-dir,
  AS_HELP_STRING(
    [--with-locale-dir=DIR],
    [Localizacion de los archivos locale [DATADIR/locale]..]
  ),[
    if test x$withval = xyes; then
      AC_MSG_WARN(El uso es: --with-locale-dir=basedir)
    else
      if test x$withval = xno; then
        AC_MSG_WARN(El uso es: --with-locale-dir=basedir)
      else
        LOCALEDIR=$withval
      fi
    fi
  ])
  AC_SUBST(LOCALEDIR)
fi

#restaurar LIBS
LIBS="$X_LIBS $temp_LIBS"

AC_PATH_PROG(XGETTEXT,xgettext,none)
XGETTEXT_TCL_WORKS=no
if test "$XGETTEXT" = "none"; then
   AC_MSG_ERROR(instalar gettext 0.14.5 o superior)
fi
AC_SUBST(XGETTEXT)

AC_PATH_PROG(MSGFMT,msgfmt,none)
if test "$MSGFMT" = "none"; then
   AC_MSG_ERROR(instalar gettext 0.14.5 o superior)
fi
AC_SUBST(MSGFMT)

if test "$PYTHON" = "none"; then
    AC_MSG_ERROR([Python desaparecido. Instálelo o especifique --disable-python para omitir las partes de LinuxCNC que dependen de Python])
fi

AC_MSG_CHECKING([python version])
if ! $PYTHON -c 'import sys;raise SystemExit(sys.hexversion<0x2070000)'
then
    AC_MSG_ERROR(version antigua de Python (se requiere 2.7 o siguientes))
fi
AC_MSG_RESULT(OK)

AC_MSG_CHECKING([version de librerias python])
LIBPYTHON=`$PYTHON -c 'import distutils.sysconfig; print("python" + (distutils.sysconfig.get_config_vars().get("LDVERSION") or distutils.sysconfig.get_config_vars().get("VERSION")))'`
AC_MSG_RESULT($LIBPYTHON)

AC_MSG_CHECKING([coincidencia entre las versiones tk y tkinter])
if $PYTHON -c 'import sys;raise SystemExit(sys.hexversion<0x3040000)'
then

	PYTHON_TK_VERSION="`$PYTHON -c 'import tkinter; print(tkinter.TkVersion)'`"
	PYTHON_TCL_VERSION="`$PYTHON -c 'import tkinter; print(tkinter.TclVersion)'`"

else

	PYTHON_TK_VERSION="`$PYTHON -c 'import _tkinter; print(_tkinter.TK_VERSION)'`"
	PYTHON_TCL_VERSION="`$PYTHON -c 'import _tkinter; print(_tkinter.TCL_VERSION)'`"

fi

if test "$PYTHON_TCL_VERSION" != "$TCL_VERSION"; then
    AC_MSG_RESULT([TCL mismatch: $TCL_VERSION vs $PYTHON_TCL_VERSION])
    AC_MSG_ERROR([Python requiere el uso de Tcl $PYTHON_TCL_VERSION y Tk $PYTHON_TK_VERSION.
Instale esta versión y especifique --with-tclConfig y --with-tkConfig si es necesario])
fi
if test "$PYTHON_TK_VERSION" != "$TK_VERSION"; then
    AC_MSG_RESULT([Tk mismatch: $TK_VERSION vs $PYTHON_TK_VERSION])
    AC_MSG_ERROR(["Python requiere el uso de Tk $TK_VERSION. Instale esta versión y especifique --with-tkConfig si es necesario"])
fi
AC_MSG_RESULT([$PYTHON_TK_VERSION])

############################

AC_MSG_CHECKING(ubicación de los archivos de cabecera de Python)
INCLUDEPY=`$PYTHON -c 'import distutils.sysconfig; print(distutils.sysconfig.get_config_vars().get("INCLUDEPY"))'`
AC_MSG_RESULT($INCLUDEPY)

AC_MSG_CHECKING([para cabeceras Python])
AS_IF([test "x$PYTHON_CPPFLAGS" = x],
[
    _PYTHON_SYSCONFIG([[get_python_inc()]], [PYTHON_CPPFLAGS="-I$py_val"])
    _PYTHON_SYSCONFIG([[get_python_inc(True)]],
    [test "x$PYTHON_CPPFLAGS" = "x-I$py_val" || PYTHON_CPPFLAGS="$PYTHON_CPPFLAGS -I$py_val"])
])
AC_MSG_RESULT([$PYTHON_CPPFLAGS])
AC_SUBST(PYTHON_CPPFLAGS)

AC_MSG_CHECKING([para librerias Python])
AS_IF([test "x$PYTHON_LIBS" = x],
[
    _PYTHON_SYSCONFIG([[get_config_var('LIBS')]], [PYTHON_LIBS=$py_val])
])
AC_MSG_RESULT([$PYTHON_LIBS])
AC_SUBST(PYTHON_LIBS)

# Obtener CFLAGS y LIBS para boost::python.
# Esto hace un AC_SUBST() de BOOST_PYTHON_LIBS
# Para CFLAGS debemos asumir que boost se encuentra en el nivel superior, por ejemplo en /usr/include/:
AX_BOOST_PYTHON_EMC

AC_CHECK_HEADER($INCLUDEPY/Python.h,[],
    [AC_MSG_ERROR([Falta encabezado requerido Python.h. Instálelo o especifique --disable-python para omitir las partes de LinuxCNC que dependen de Python])])

AC_MSG_CHECKING(para ubicacion de site-package)
SITEPY=`$PYTHON -c 'import distutils.sysconfig; print(distutils.sysconfig.get_python_lib())'`
AC_MSG_RESULT($SITEPY)

AC_CHECK_HEADERS(GL/gl.h GL/glu.h,[],[AC_MSG_ERROR([Falta el encabezado de OpenGL requerido. Instálelo o especifique --disable-python para omitir las partes de LinuxCNC que dependen de Python])])

AC_CHECK_LIB(GL, glBegin, [], [AC_MSG_ERROR([Falta la biblioteca de GL requerida. Instálelo o especifique --disable-python para omitir las partes de LinuxCNC que dependen de Python])])

AC_MSG_CHECKING(para cuadráticas GLU)
AC_TRY_COMPILE([
#include <GL/gl.h>
#include <GL/glu.h>
],
    [GLUquadric *q;],
    [AC_MSG_RESULT(yes)],[
            AC_MSG_ERROR([Falta la biblioteca de GLU requerida. Instálelo o especifique --disable-python para omitir las partes de LinuxCNC que dependen de Python]])]
    
)

AC_MSG_CHECKING(para headers Xmu)
AC_CHECK_HEADERS(X11/Xmu/Xmu.h,[],[AC_MSG_ERROR([Falta cabeceras Xmu requerida. Instálelo o especifique --disable-python para omitir las partes de LinuxCNC que dependen de Python])])

AC_SUBST([LIBPYTHON])
AC_SUBST([INCLUDEPY])
AC_SUBST([SITEPY])


##############################################################################
# files that get created by ./configure                                      #
##############################################################################
#AC_CONFIG_FILES([../configs/rtapi.conf ../configs/emc.conf ../configs/hal.conf config.h])
AC_CONFIG_FILES([../scripts/rtapi.conf])
AC_CONFIG_FILES([../scripts/linuxcnc], [chmod +x ../scripts/linuxcnc])
AC_CONFIG_FILES([../scripts/halrun], [chmod +x ../scripts/halrun])
AC_CONFIG_FILES([../scripts/rip-environment], [chmod +x ../scripts/rip-environment])
AC_CONFIG_FILES([../scripts/haltcl], [chmod +x ../scripts/haltcl])
AC_CONFIG_FILES([../scripts/halcmd_twopass], [chmod +x ../scripts/halcmd_twopass])
AC_CONFIG_FILES([../scripts/realtime], [chmod +x ../scripts/realtime])
AC_CONFIG_FILES([../scripts/linuxcnc_var], [chmod +x ../scripts/linuxcnc_var])
AC_CONFIG_FILES(Makefile.inc)
AC_CONFIG_FILES(Makefile.modinc)
AC_CONFIG_FILES(../tcl/linuxcnc.tcl)
AC_CONFIG_FILES(../lib/python/nf.py)
AC_CONFIG_FILES([../scripts/linuxcncmkdesktop], [chmod +x ../scripts/linuxcncmkdesktop])
AC_CONFIG_FILES(../share/applications/linuxcnc-latency.desktop)
AC_CONFIG_FILES(../share/applications/linuxcnc.desktop)
AC_CONFIG_FILES(../share/desktop-directories/cnc.directory)
AC_CONFIG_FILES(../share/menus/CNC.menu)
AC_OUTPUT()

##############################################################################
# mensaje al usuario sobre qué hacer, después de ./configure con exito       # 
##############################################################################
bold () {
    if tty > /dev/null 2>&1 && type -path tput > /dev/null 2>&1;
    then
        tput smso
    fi
}

offbold () {
    if tty > /dev/null 2>&1 && type -path tput > /dev/null 2>&1;
    then
        tput rmso
    fi
}

echo ""
echo ""
echo "######################################################################"
echo "#             LinuxCNC - Controlador de máquina mejorado             #"
echo "######################################################################"
echo "#                                                                    #"
echo "# LinuxCNC es un sistema de software para el control informático     #"
echo "# de máquinas herramientas tales como fresadoras. LinuxCNC se lanza  #"
echo "# bajo GPL. Vea http://www.linuxcnc.org/ para obtener más detalles.  #"
echo "#                                                                    #"
echo "#                                                                    #"
echo "# Parece que ./configure se completó con exito.                      #"
echo "# Esto significa que RT está correctamente instalado                 #"
echo "# Si las cosas no funcionan, compruebe si hay errores y advertencias #"
echo "# en config.log                                                      #"
echo "#                                                                    #"
if test "xyes" = "x$RUN_IN_PLACE"; then
echo "#   Ahora, compile escribiendo:                                      #"
echo "#         make                                                       #"
echo "#         sudo make setuid                                           #"
if test $RTS = uspace; then
echo "#          (si se requiere tiempo real y acceso al hardware)         #"
fi
echo "#                                                                    #"
echo "#   Antes de ejecutar el software, configure el entorno:             #"
echo "#         . (top dir)/scripts/rip-environment                        #"
else
bold
echo "#   Advertencia: si ya tiene instalado linuxcnc, esto                #"
echo "#         reemplaza una instalación existente. Si ha instalado       #"
echo "#         un paquete linuxcnc, esto dañará el paquete.               #"
offbold
echo "#   Sugerencia: para probar una versión de linuxcnc compilada        #"
echo "#   sin dañar la versión del paquete, no especifique un --prefix     #"
echo "#                                                                    #"
echo "#   Ahora compile escribiendo:                                       #"
echo "#         make                                                       #"
echo "#   despues, instale con:                                            #"
echo "#         sudo make install                                          #"
fi
echo "#                                                                    #"
echo "#   Para ejecutar el software escriba:                               #"
echo "#         linuxcnc                                                   #"
echo "#                                                                    #"
echo "######################################################################"
echo ""
echo ""
