# basic_sim.tcl
#
# Proporciona componentes y conexiones hal comunes para una máquina simulada.
# Por defecto, el script crea y conecta componentes ddts, simulated_home,
# husillo y hal_manualtoolchange.
# #
# Las opciones hay para
# 	a) deshabilitar componentes y conexiones hal específicos
# 	b) guardar un halfile equivalente a los comandos hal ejecutados por
# 	   este script (y cualquier comando hal ejecutado anteriormente)
# #
# Las letras de coordenadas y el número de articulaciones se determinan a 
# partir de la configuración del archivo ini habitual
# #
# Uso del archivo Ini:
# 				[HAL]HALFILE=basic_sim.tcl [Opciones]
# Opciones:
# 				-no_make_ddts
# 				-no_simulated_home
# 				-no_use_hal_manualtoolchange
# 				-no_sim_spindle

# ------------------------------------------------- ---------------------
# Notas:
# 1) ::env() es una matriz asociativa global de variables de entorno
# 	 exportadas por el script principal LinuxCNC (linuxcnc)
# 2) Las configuraciones del archivo ini están disponibles como arrays asociativos globales
#    nombrados: ::SECTION(varname)
# 		ejemplo: :: EMCMOT (SERVO_PERIOD)
# 3) los procs son de sim_lib.tcl
#begin-----------------------------------------------------------------
source [file join $::env(HALLIB_DIR) sim_lib.tcl]
set save_options {}

if [catch {check_ini_items} msg] {
  puts "\nbasic_sim.tcl ERROR: $msg\n"
  exit 1
}
set axes [get_traj_coordinates]
set number_of_joints $::KINS(JOINTS)

set base_period 0 ;# 0 significa sin hilo
if [info exists ::EMCMOT(BASE_PERIOD)] {
  set base_period $::EMCMOT(BASE_PERIOD)
}

core_sim $axes \
         $number_of_joints \
         $::EMCMOT(SERVO_PERIOD) \
         $base_period \
         $::EMCMOT(EMCMOT)

if {[lsearch -nocase $::argv -no_make_ddts] < 0} {
  make_ddts $number_of_joints
}
if {[lsearch -nocase $::argv -no_simulated_home] < 0} {
  simulated_home $number_of_joints
}
if {[lsearch -nocase $::argv -no_use_hal_manualtoolchange] < 0} {
  use_hal_manualtoolchange
  lappend save_options use_hal_manualtoolchange
}
if {[lsearch -nocase $::argv -no_sim_spindle] < 0} {
  sim_spindle
}

# hacer un halfile (inifilename_cmds.hal) con comandos hal equivalentes
set savefilename \
    ./[file rootname [file tail $::env(INI_FILE_NAME)]]_cmds.hal
save_hal_cmds $savefilename $save_options
