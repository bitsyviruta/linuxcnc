# check_config.tcl
# Uso: tclsh path_to_this_file inifilename
# si falla un chequeo, imprime el mensaje y sale con 1
# #
# chequeos:
# 	1) elementos obligatorios por lista ::mandatory_items
# 	2) si trivkins, verifica la consistencia de [JOINT_] y [AXIS_]
#      MIN_LIMIT y MAX_LIMIT
#----------------------------------------------------------------------
set ::mandatory_items {KINS KINEMATICS
                       KINS JOINTS
                      }
# Ref: src/emc/nml_intf/emccfg.h,src/emc/ini/inijoint.cc,src/emc/iniaxis.cc:
set ::DEFAULT_AXIS_MAX_VELOCITY      1.0
set ::DEFAULT_AXIS_MAX_ACCELERATION  1.0
set ::DEFAULT_JOINT_MAX_VELOCITY     1.0
set ::DEFAULT_JOINT_MAX_ACCELERATION 1.0

# Ref: src/emc/ini/iniaxis.cc:
set ::DEFAULT_AXIS_MIN_LIMIT -1e99
set ::DEFAULT_AXIS_MAX_LIMIT +1e99
#----------------------------------------------------------------------
proc warnings msg {
  puts "\n$::progname: ($::kins(module) kinematics) AVISO:"
  foreach m $msg {puts "  $m"}
  puts ""
} ;# warnings

proc err_exit msg {
  puts "\n$::progname: ($::kins(module) kinematics) ERROR:"
  foreach m $msg {puts "  $m"}
  puts ""
  exit 1
} ;# err_exit

proc check_mandatory_items {} {
  foreach {section item} $::mandatory_items {
    if ![info exists ::[set section]($item) ] {
      set section [string trim $section :]
      lappend emsg "No encontrado \[$section\]$item="
    }
  }
  if [info exists emsg] {
    err_exit $emsg
  }
} ;# check_mandatory_items

proc uniq {list_name} {  ;# hacer lista unica
  foreach item $list_name { set tmp($item) "" }
  return [array names tmp]
} ;# uniq

proc joints_for_trivkins {coords} {
  # ref: src/emc/kinematics/trivkins.c
  # el orden de letras de coordenada determina el numero de articulacion asignada
  if {"$coords" == ""} {set coords {X Y Z A B C U V W}}
  set i 0
  foreach a [string toupper $coords] {
     # asignar a articulaciones consecutivas:
     lappend ::kins(jointidx,$a) $i
     incr i
  }
} ;# joints_for_trivkins

proc consistent_coords_for_trivkins {trivcoords} {
  set m {" " "" "\t" ""} ;# mapeo para remover espacios en blanco
  set trivcoords [string map $m $trivcoords]
  if {"$trivcoords" == "XYZABCUVW"} {
     return ;# coordenadas trivkins inespecificas
             # permiten el uso de cualquier coordenada
  }
  set trajcoords ""
  if [info exists ::TRAJ(COORDINATES)] {
    set trajcoords [string map $m [lindex $::TRAJ(COORDINATES) 0]]
  }
  if {[string toupper "$trivcoords"] != [string toupper "$trajcoords"]} {
    lappend ::wmsg "Especificacion de coordenadas INCONSISTENTE:
               trivkins coordinates=$trivcoords
               \[TRAJ\]COORDINATES=$trajcoords"
  }
} ;# consistent_coords_for_trivkins

proc validate_identity_kins_limits {} {
  set emsg ""
  for {set j 0} {$j < $::KINS(JOINTS)} {incr j} {
    if {![info exists ::JOINT_[set j](MAX_VELOCITY)] } {
      lappend ::wmsg "Sin especificar \[JOINT_$j\]MAX_VELOCITY,     por defecto: $::DEFAULT_JOINT_MAX_VELOCITY"
    }
    if {![info exists ::JOINT_[set j](MAX_ACCELERATION)] } {
      lappend ::wmsg "Sin especificar \[JOINT_$j\]MAX_ACCELERATION, por defecto: $::DEFAULT_JOINT_MAX_ACCELERATION"
    }
  }
  foreach c [uniq $::kins(coordinates)] {
    # array test avoids superfluous messages when coordinates= not specified
    if [array exists ::AXIS_[set c]] {
      if {![info exists ::AXIS_[set c](MAX_VELOCITY)] } {
        lappend ::wmsg "Sin especificar \[AXIS_$c\]MAX_VELOCITY,     por defecto: $::DEFAULT_AXIS_MAX_VELOCITY"
      }
      if {![info exists ::AXIS_[set c](MAX_ACCELERATION)] } {
        lappend ::wmsg "Sin especificar \[AXIS_$c\]MAX_ACCELERATION, por defecto: $::DEFAULT_AXIS_MAX_ACCELERATION"
      }
    }

    set missing_axis_min_limit 0
    set missing_axis_max_limit 0
    if ![info exists ::AXIS_[set c](MIN_LIMIT)] {
      set ::AXIS_[set c](MIN_LIMIT) $::DEFAULT_AXIS_MIN_LIMIT
      set missing_axis_min_limit 1
    }
    if ![info exists ::AXIS_[set c](MAX_LIMIT)] {
      set ::AXIS_[set c](MAX_LIMIT) $::DEFAULT_AXIS_MAX_LIMIT
      set missing_axis_max_limit 1
    }
    foreach j $::kins(jointidx,$c) {
      if [info exists  ::JOINT_[set j](MIN_LIMIT)] {
         set jlim [set ::JOINT_[set j](MIN_LIMIT)]
         set clim [set  ::AXIS_[set c](MIN_LIMIT)]
         if {$jlim > $clim} {
           if $missing_axis_min_limit {
             lappend ::wmsg "Sin especificar \[AXIS_$c\]MIN_LIMIT,        por defecto: $::DEFAULT_AXIS_MIN_LIMIT"
           }
           lappend emsg "\[JOINT_$j\]MIN_LIMIT > \[AXIS_$c\]MIN_LIMIT ($jlim > $clim)"
         }
      }
      if [info exists  ::JOINT_[set j](MAX_LIMIT)] {
         set jlim [set ::JOINT_[set j](MAX_LIMIT)]
         set clim [set  ::AXIS_[set c](MAX_LIMIT)]
         if {$jlim < $clim} {
           if $missing_axis_max_limit {
             lappend ::wmsg "Sin especificar \[AXIS_$c\]MAX_LIMIT,        por defecto: $::DEFAULT_AXIS_MAX_LIMIT"
           }
           lappend emsg "\[JOINT_$j\]MAX_LIMIT < \[AXIS_$c\]MAX_LIMIT ($jlim < $clim)"
         }
      }
    }
  }
  return $emsg
} ;# validate_identity_kins_limits

proc check_extrajoints {} {
  if ![info exists ::EMCMOT(EMCMOT)] return
  if {[string first motmod $::EMCMOT(EMCMOT)] <= 0} return
  set mot [split [lindex $::EMCMOT(EMCMOT) 0]]
  foreach item $mot {
    set pair [split $item =]
    set parm [lindex $pair 0]
    set val  [lindex $pair 1]
    if {"$parm" == "num_extrajoints"} {
      set ::num_extrajoints $val
    }
  }
  if [info exists ::num_extrajoints] {
     lappend ::wmsg [format "Articulaciones Extra especificadas=%d\n \
\[KINS\]JOINTS=%d debe acomodar articulaciones cinematicas*mas* articulaciones extra " \
                     $::num_extrajoints $::KINS(JOINTS)]
  }
} ;#check_extrajoints
#----------------------------------------------------------------------
# begin
package require Linuxcnc ;# parse_ini

set ::progname [file rootname [file tail [info script]]]
set inifile [lindex $::argv 0]
if ![file exists $inifile]   {
  err_exit [list "No existe el archivo <$inifile>"]
}
if ![file readable $inifile] {
   err_exit [list "Archivo no legible <$inifile>"]
}
parse_ini $inifile

check_mandatory_items

set kins_kinematics [lindex $::KINS(KINEMATICS) end]
set ::kins(module)  [lindex $kins_kinematics 0]

# parametros especificados como parm=value --> ::kins(parm)=value
foreach item $kins_kinematics {
  if {[string first = $item] >= 0} {
    set l [split $item =]
    set ::kins([lindex $l 0]) [lindex $l 1]
  } else {
    if {"$item" != $::kins(module)} {
      puts "Desconocido \[KINS\]KINEMATICS item: <$item>"
    }
  }
}
# coordinates --> ::kins(coordinates) (todas, si no se especifica)
if [info exists ::kins(coordinates)] {
  set ::kins(coordinates) [split $::kins(coordinates) ""]
  set ::kins(coordinates) [string toupper $::kins(coordinates)]
} else {
  set ::kins(coordinates) {X Y Z A B C U V W}
}

# lista de articulaciones para cada coordenada --> ::kins(jointidx,coordinateletter)
switch $::kins(module) {
  trivkins {joints_for_trivkins $::kins(coordinates)}
  default  {
    puts "$::progname: Sin chequeo: \[KINS\]KINEMATICS=$::KINS(KINEMATICS)"
    exit 0
  }
}
check_extrajoints


#parray ::kins
set emsg [validate_identity_kins_limits]
consistent_coords_for_trivkins $::kins(coordinates)
if [info exists ::wmsg] {warnings $::wmsg}

if {"$emsg" == ""} {exit 0}
err_exit $emsg
