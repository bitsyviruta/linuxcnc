/********************************************************************
* Descripcion: interp_cycles.cc
*
*   La mayor parte de estas funciones controlan cómo se interpretan 
*   los ciclos fijos.
*
* Author:
* License: GPL Version 2
* System: Linux
*    
* Copyright (c) 2004 All rights reserved.
*
********************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rs274ngc.hh"
#include "rs274ngc_return.hh"
#include "interp_internal.hh"
#include "rs274ngc_interp.hh"

static const char* plane_name(CANON_PLANE p);

/****************************************************************************/

/*! convert_cycle_g81

Valor de retorno: int (INTERP_OK)

efectos: Ver a continuacion

Llamada por:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

Para el plano XY, esto implementa el siguiente ciclo RS274/NGC, que 
generalmente es de taladrado:
1. Mueve solo el eje z a la velocidad de avance actual al bottom_z especificado.
2. Retrae el eje z a velocidad rapida a clear_z.

Ver [NCMS, page 99].

CYCLE_MACRO ha posicionado la herramienta en (x, y, r, a, b, c) cuando esto comienza.

Para los planos XZ e YZ, esto hace movimientos análogos.

*/

int Interp::convert_cycle_g81(block_pointer block,
                              CANON_PLANE plane, //!< plano seleccionado
                              double x,  //!< x-valor donde se ejecuta el ciclo
                              double y,  //!< y-valor donde se ejecuta el ciclo
                              double clear_z,    //!< z-valor del plano libre      
                              double bottom_z)   //!< z-valor al fondo del ciclo   
{
    cycle_feed(block, plane, x, y, bottom_z);
    cycle_traverse(block, plane, x, y, clear_z);

  return INTERP_OK;
}

/****************************************************************************/

/*! convert_cycle_g82

Valor retornado: int (INTERP_OK)

efectos: Ver a continuacion

Llamado por:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

Para el plano XY, esto implementa el siguiente ciclo RS274/NGC, que 
generalmente es de taladrado:
1. Mueve solo el eje z a la velocidad de avance actual al z-value especificado.
2. Dwell para el numero de segundos dado.
3. Retrae el eje z a velocidad rapida a clear_z.

CYCLE_MACRO ha posicionado la herramienta en (x, y, r, a, b, c) cuando esto comienza.

Para los planos XZ e YZ, esto hace movimientos análogos.

*/

int Interp::convert_cycle_g82(block_pointer block,
                              CANON_PLANE plane, //!< selected plane                  
                              double x,  //!< x-valor donde se ejecuta el ciclo
                              double y,  //!< y-valor donde se ejecuta el ciclo
                              double clear_z,    //!< z-valor del plano libre      
                              double bottom_z,   //!< z-valor al fondo del ciclo  
                              double dwell)      //!< tiempo dwell                      
{
  cycle_feed(block, plane, x, y, bottom_z);
  DWELL(dwell);
  cycle_traverse(block, plane, x, y, clear_z);

  return INTERP_OK;
}

/****************************************************************************/

/*! convert_cycle_g83

Valor retornado: int (INTERP_OK)

efectos: Ver a continuacion

Llamado por:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

Para el plano XY, esto implementa el siguiente ciclo RS274/NGC, que 
generalmente es de taladrado con picado:
1. Mueve solo el eje z a la velocidad de avance actual delta hacia abajo o
   al bottom_z especificado, lo que sea menos profundo.
2. Rápido de regreso al plano R.
3. Rápido al fondo del agujero actual, retrocediendo un poco.
4. Repetir los pasos 1, 2 y 3 hasta alcanzar el bottom_z especificado.
5. Retraer el eje z a veloccidad rapida a clear_z.

CYCLE_MACRO ha posicionado la herramienta en (x, y, r, a, b, c) cuando esto comienza.

La salida y la entrada rápidas causan que las virutas largas (comunes
al perforar en aluminio) se corten y despeja las virutas del
agujero.

Para los planos XZ e YZ, esto hace movimientos análogos.

*/

int Interp::convert_cycle_g83(block_pointer block,
                              CANON_PLANE plane, //!< plano seleccionado                 
                              double x,  //!< x-valor donde se ejecuta el ciclo
                              double y,  //!< y-valor donde se ejecuta el ciclo
                              double r,  //!< z-valor inicial                 
                              double clear_z,    //!< z-valor de plano libre     
                              double bottom_z,   //!< z-valor al fondo del ciclo   
                              double delta)      //!< incremento de avance en eje z 
{
  double current_depth;
  double rapid_delta;

/*   Se movió aqui la comprobación de valores Q negativos ya 
     que se puede usar un signo con funciones M definidas por el usuario
      Gracias a Billy Singleton por señalarlo ... */
  CHKS((delta <= 0.0), NCE_NEGATIVE_OR_ZERO_Q_VALUE_USED);

  rapid_delta = G83_RAPID_DELTA;
  if (_setup.length_units == CANON_UNITS_MM)
    rapid_delta = (rapid_delta * 25.4);

  for (current_depth = (r - delta);
       current_depth > bottom_z; current_depth = (current_depth - delta)) {
    cycle_feed(block, plane, x, y, current_depth);
    cycle_traverse(block, plane, x, y, r);
    cycle_traverse(block, plane, x, y, current_depth + rapid_delta);
  }
  cycle_feed(block, plane, x, y, bottom_z);
  cycle_traverse(block, plane, x, y, clear_z);

  return INTERP_OK;
}

/****************************************************************************/

/*! convert_cycle_g73

Valor retornado: int (INTERP_OK)

efectos: Ver a continuacion

Llamado por
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

Para el plano XY, esto implementa el siguiente ciclo RS274/NGC, que 
generalmente es de taladrado con picado:
1. Mueve solo el eje z a la velocidad de avance actual delta hacia abajo o
   al bottom_z especificado, lo que sea menos profundo.
2. Rápido retrocediendo un poco.
3. Repetir los pasos 1, 2 y 3 hasta alcanzar el bottom_z especificado.
4. Retraer el eje z a veloccidad rapida a clear_z.

CYCLE_MACRO ha posicionado la herramienta en (x, y, r, a, b, c) cuando esto comienza.

La salida y la entrada rápidas causan que las virutas largas (comunes
al perforar en aluminio) se corten y despeja las virutas del
agujero.

Para los planos XZ e YZ, esto hace movimientos análogos.

*/

int Interp::convert_cycle_g73(block_pointer block,
                              CANON_PLANE plane, //!< plano seleccionado                 
                              double x,  //!< x-valor donde se ejecuta el ciclo
                              double y,  //!< y-valor donde se ejecuta el ciclo
                              double r,  //!< z-valor inicial                 
                              double clear_z,    //!< z-valor de plano libre     
                              double bottom_z,   //!< z-valor al fondo del ciclo   
                              double delta)      //!< incremento de avance en eje z    
{
  double current_depth;
  double rapid_delta;

/*   Se movió aqui la comprobación de valores Q negativos ya 
     que se puede usar un signo con funciones M definidas por el usuario
      Gracias a Billy Singleton por señalarlo ... */
  CHKS((delta <= 0.0), NCE_NEGATIVE_OR_ZERO_Q_VALUE_USED);

  rapid_delta = G83_RAPID_DELTA;
  if (_setup.length_units == CANON_UNITS_MM)
    rapid_delta = (rapid_delta * 25.4);

  for (current_depth = (r - delta);
       current_depth > bottom_z; current_depth = (current_depth - delta)) {
    cycle_feed(block, plane, x, y, current_depth);
    cycle_traverse(block, plane, x, y, current_depth + rapid_delta);
  }
  cycle_feed(block, plane, x, y, bottom_z);
  cycle_traverse(block, plane, x, y, clear_z);

  return INTERP_OK;
}

/****************************************************************************/

/*! convert_cycle_g74_g84

Valor retornado: int
   Si el husillo no gira en sentido horario y el movimiento no es g84, esto devuelve
      NCE_SPINDLE_NOT_TURNING_CLOCKWISE_IN_G84.
   Si el husillo no gira en sentido antihorario y el movimiento no es g74, esto vuelve
      NCE_SPINDLE_NOT_TURNING_COUNTER_CLOCKWISE_IN_G74.

Efectos: ver a continuacion

Llamado por:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

Para el plano XY, esto implementa el siguiente ciclo RS274/NGC,
+que es el roscado con mandril flotante a derechas:
1. Inicia el avance hacia z_abajo.
2. Mueve solo el eje z a la velocidad de avance actual al bottom_z especificado.
3. Detiene el husillo.
4. Arranca el husillo en sentido antihorario (o cw si g74), suprimiendo la
   espera de husillo a velocidad para el siguiente avance de alimentación.
5. Retrae el eje z a la velocidad de avance actual a clear_z.
6. Detiene el husillo.
7. Arranca el husillo en el sentido de las agujas del reloj (o hacia la izquierda si g74), con espera normal de velocidad para el próximo movimiento de alimentación.

CYCLE_MACRO ha posicionado la herramienta en (x, y, r, a, b, c) cuando esto comienza.
El argumento de dirección debe ser en sentido horario.

Para los planos XZ e YZ, hace movimientos análogos.
*/

InterpReturn Interp::check_g74_g84_spindle(GCodes motion, CANON_DIRECTION dir)
{
    switch (dir) {
    case CANON_STOPPED:
        ERS(_("Spindle not turning in %s"), toString((GCodes)motion).c_str());
    case CANON_CLOCKWISE:
        CHKS((motion == G_74), _("Spindle turning clockwise in G74"));
        return INTERP_OK;
    case CANON_COUNTERCLOCKWISE:
        CHKS((motion == G_84), _("Spindle turning counterclockwise in G84"));
        return INTERP_OK;
    }
    ERS(_("Spindle state unknown for %s"), toString(motion).c_str());
}

int Interp::convert_cycle_g74_g84(block_pointer block,
                                 CANON_PLANE plane, //!< Plano seleccionado
                                 double x,  //!< valor-x donde se ejecuta el ciclo
                                 double y,  //!< valor-y donde se ejecuta el ciclo
                                 double clear_z,    //!< valor z del plano libre
                                 double bottom_z,   //!< valor de z de la parte inferior del ciclo
                                 CANON_DIRECTION direction, //!< dirección de giro al principio
                                 CANON_SPEED_FEED_MODE mode,       //!< modo de velocidad de avance al comienzo
                                 int motion, double dwell, int spindle)
{

    CHP(check_g74_g84_spindle((GCodes)motion, direction));

    int save_feed_override_enable;
    int save_spindle_override_enable;

    save_feed_override_enable = GET_EXTERNAL_FEED_OVERRIDE_ENABLE();
    save_spindle_override_enable = GET_EXTERNAL_SPINDLE_OVERRIDE_ENABLE(spindle);

   switch (plane) {

    case CANON_PLANE_XY:
       DISABLE_FEED_OVERRIDE();
       DISABLE_SPEED_OVERRIDE(spindle);
       cycle_feed(block, plane, x, y, bottom_z);
       STOP_SPINDLE_TURNING(spindle);
       // el parámetro cero suprime la espera de a-velocidad en la próxima alimentación
       if (motion == G_84)
           START_SPINDLE_COUNTERCLOCKWISE(spindle);
       else
           START_SPINDLE_CLOCKWISE(spindle);
       DWELL(dwell);
       cycle_feed(block, plane, x, y, clear_z);
       STOP_SPINDLE_TURNING(spindle);
       if (motion == G_84)
           START_SPINDLE_CLOCKWISE(spindle);
       else
           START_SPINDLE_COUNTERCLOCKWISE(spindle);
       break;

    case CANON_PLANE_YZ:
       DISABLE_FEED_OVERRIDE();
       DISABLE_SPEED_OVERRIDE(spindle);
       cycle_feed(block, plane, bottom_z, x, y);
       STOP_SPINDLE_TURNING(spindle);
       if (motion == G_84)
           START_SPINDLE_COUNTERCLOCKWISE(spindle);
       else
           START_SPINDLE_CLOCKWISE(spindle);
       DWELL(dwell);
       cycle_feed(block, plane, clear_z, x, y);
       STOP_SPINDLE_TURNING(spindle);
       if (motion == G_84)
           START_SPINDLE_CLOCKWISE(spindle);
       else
           START_SPINDLE_COUNTERCLOCKWISE(spindle);
       break;

    case CANON_PLANE_XZ:
       DISABLE_FEED_OVERRIDE();
       DISABLE_SPEED_OVERRIDE(spindle);
       cycle_feed(block, plane, y, bottom_z, x);
       STOP_SPINDLE_TURNING(spindle);
       if (motion == G_84)
           START_SPINDLE_COUNTERCLOCKWISE(spindle);
       else
           START_SPINDLE_CLOCKWISE(spindle);
       DWELL(dwell);
       cycle_feed(block, plane, y, clear_z, x);
       STOP_SPINDLE_TURNING(spindle);
       if (motion == G_84)
           START_SPINDLE_CLOCKWISE(spindle);
       else
           START_SPINDLE_COUNTERCLOCKWISE(spindle);
       break;

    default:
       ERS("%s for plane %s not implemented",
           toString((GCodes)motion).c_str(), plane_name(plane));
    }
   if(save_feed_override_enable)
    ENABLE_FEED_OVERRIDE();
   if(save_spindle_override_enable)
    ENABLE_SPEED_OVERRIDE(spindle);

   return INTERP_OK;

#if 0
  START_SPEED_FEED_SYNCH();
  cycle_feed(block, plane, x, y, bottom_z);

  cycle_feed(block, plane, x, y, clear_z);
  if (mode != CANON_SYNCHED)
    STOP_SPEED_FEED_SYNCH();
  STOP_SPINDLE_TURNING();
  START_SPINDLE_CLOCKWISE();
#endif
}

/****************************************************************************/

/*! convert_cycle_g85

Valor retornado: int (INTERP_OK)

Efectos secundarios:
   Se realizan varios movimientos como se describe a continuación.

Llamado por:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

Para el plano XY, esto implementa el siguiente ciclo RS274/NGC,
que suele ser mandrinado o escariado:
1. Mover solo el eje z a la velocidad de avance actual al valor z especificado.
2. Retraer el eje z a la velocidad de avance actual a clear_z.

CYCLE_MACRO ha posicionado la herramienta en (x, y, r,?,?) Cuando esto comienza.

Para los planos XZ e YZ, esto hace movimientos análogos.

*/

int Interp::convert_cycle_g85(block_pointer block,
                              CANON_PLANE plane, //!< Plano seleccionado                  
                              double x,  //!< valor-x donde se ejecuta el ciclo 
                              double y,  //!< valor-y donde se ejecuta el ciclo
                              double r,  // plano de retraccion
                              double clear_z,    //!< valor z del plano libre      
                              double bottom_z)   //!< valor z al fondo del ciclo
{
  cycle_feed(block, plane, x, y, bottom_z);
  cycle_feed(block, plane, x, y, r);
  cycle_traverse(block, plane, x, y, clear_z);

  return INTERP_OK;
}

/****************************************************************************/

/*! convert_cycle_g86

Valor retornado: int
   Si el husillo no gira en sentido horario o antihorario,
   esto devuelve NCE_SPINDLE_NOT_TURNING_IN_G86.
   De lo contrario, devuelve INTERP_OK.

Efectos secundarios:
   Se realizan varios movimientos como se describe a continuación.

Llamado por:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

Para el plano XY, esto implementa el siguiente ciclo RS274 / NGC,
el cual suele ser mandrinado:
1. Mover solo el eje z a la velocidad de avance actual a bottom_z.
2. Permanecer el número de segundos dado.
3. Detener el giro del husillo.
4. Retraer el eje z a la velocidad transversal a clear_z.
5. Reiniciar el husillo en la dirección en la que iba.

CYCLE_MACRO ha posicionado la herramienta en (x, y, r, a, b, c) cuando esto comienza.

Para los planos XZ e YZ, esto hace movimientos análogos.

*/

int Interp::convert_cycle_g86(block_pointer block,
                              CANON_PLANE plane, //!< Plano seleccionado               
                              double x,  //!< valor-x donde se ejecuta el ciclo 
                              double y,  //!< valor-x donde se ejecuta el ciclo    
                              double clear_z,    //!< valor z del plano libre      
                              double bottom_z,   //!< valor de z en el fondo del ciclo     
                              double dwell,      //!< tiempo dwell
                              CANON_DIRECTION direction, //!< dirección de giro al principio
                              int spindle)       // husillo en uso
{
  CHKS(((direction != CANON_CLOCKWISE) &&
       (direction != CANON_COUNTERCLOCKWISE)),
      NCE_SPINDLE_NOT_TURNING_IN_G86);

  cycle_feed(block, plane, x, y, bottom_z);
  DWELL(dwell);
  STOP_SPINDLE_TURNING(spindle);
  cycle_traverse(block, plane, x, y, clear_z);
  if (direction == CANON_CLOCKWISE)
    START_SPINDLE_CLOCKWISE(spindle);
  else
    START_SPINDLE_COUNTERCLOCKWISE(spindle);

  return INTERP_OK;
}

/****************************************************************************/

/*! convert_cycle_g87

Valor retornado: int
   Si el eje no gira en sentido horario o antihorario,
   esto devuelve NCE_SPINDLE_NOT_TURNING_IN_G87.
   De lo contrario, devuelve INTERP_OK.

Efectos secundarios:
   Se realizan varios movimientos como se describe a continuación. Este ciclo es un
   versión modificada de [Monarch, página 5-24] ya que [NCMS, páginas 98 - 100]
   no da idea de lo que se supone que debe hacer el ciclo. [KT] no
   tiene un ciclo de mandrinado trasero. [Fanuc, página 132] en "Ciclo fijo II"
   describe el ciclo G87 como se indica aquí, excepto que la dirección de
   el giro del husillo siempre es en sentido horario y se omite el paso 7.

Llamado por:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

Para el plano XY, esto implementa el siguiente ciclo RS274/NGC, que
suele ser mandrinado. La situación es que se tiene un agujero pasante
y se quiere un alojamiento en el fondo del agujero. Para hacer esto, se pone una
herramienta en forma de L en el husillo con una superficie de corte en el lado SUPERIOR
de su base. Se pasa cuidadosamente a través del agujero cuando no está
girando y orientada para que pase por el agujero. Luego se mueve
el vástago de la L hasta el eje del agujero, arranca el husillo,
y se alimenta la herramienta hacia arriba para hacer el alojamiento. Despues se saca la
herramienta fuera del agujero.

1. Mover a velocidad rapida paralela al plano XY al punto
   con el valor x offset_x y el valor y offset_y.
2. Detener el husillo en una orientación específica.
3. Mover solo el eje z a velocidad rapida hacia abajo hasta bottom_z.
4. Mover a velocidad rapida paralelo al plano XY a la ubicación x, y.
5. Arrancar el husillo en la dirección en la que iba antes.
6. Mover solo el eje z a la velocidad de avance dada hacia arriba a middle_z.
7. Mover el eje z solo, a la velocidad de avance dada, de nuevo a bottom_z.
8. Detener el husillo en la misma orientación que antes.
9. Mover a velocidad rapida, paralelo al plano XY, al punto
   con el valor x offset_x y el valor y offset_y.
10. Mover solo el eje z a velocidad rapida hasta el valor z libre.
11. Mover a velocidad rapida paralelo al plano XY al x, y especificado.
12. Reiniciar el husillo en la dirección en que iba antes.

CYCLE_MACRO ha posicionado la herramienta en (x, y, r, a, b, c) antes de que esto comience.

Puede ser útil agregar un chequeo de que clear_z > middle_z > bottom_z.
Sin el chequeo, sin embargo, esto se puede usar para hacer un agujero en
material al que solo se puede acceder a través de un agujero en el material que se encuentra sobre él.

Para los planos XZ e YZ, esto hace movimientos análogos.

*/

int Interp::convert_cycle_g87(block_pointer block,
                              CANON_PLANE plane, //! <Plano seleccionado
                              double x, //! <valor-x donde se ejecuta el ciclo
                              double offset_x, //! <offset de posición del eje x
                              double y, //! <valor de y donde se ejecuta el ciclo
                              double offset_y, //! <offset de posición del eje y
                              double r, //! <valor_z de plano_r
                              double clear_z, //! <valor z del plano libre
                              double middle_z, //! <valor z de la parte superior del orificio posterior
                              double bottom_z, //! <valor de z en la parte inferior del ciclo
                              CANON_DIRECTION direction, //! <dirección giro de husillo al principio
                              int spindle) // el husillo en uso
{
  CHKS(((direction != CANON_CLOCKWISE) &&
       (direction != CANON_COUNTERCLOCKWISE)),
      NCE_SPINDLE_NOT_TURNING_IN_G87);

  cycle_traverse(block, plane, offset_x, offset_y, r);
  STOP_SPINDLE_TURNING(spindle);
  ORIENT_SPINDLE(spindle, 0.0, direction);
  cycle_traverse(block, plane, offset_x, offset_y, bottom_z);
  cycle_traverse(block, plane, x, y, bottom_z);
  if (direction == CANON_CLOCKWISE)
    START_SPINDLE_CLOCKWISE(spindle);
  else
    START_SPINDLE_COUNTERCLOCKWISE(spindle);
  cycle_feed(block, plane, x, y, middle_z);
  cycle_feed(block, plane, x, y, bottom_z);
  STOP_SPINDLE_TURNING(spindle);
  ORIENT_SPINDLE(spindle,0.0, direction);
  cycle_traverse(block, plane, offset_x, offset_y, bottom_z);
  cycle_traverse(block, plane, offset_x, offset_y, clear_z);
  cycle_traverse(block, plane, x, y, clear_z);
  if (direction == CANON_CLOCKWISE)
    START_SPINDLE_CLOCKWISE(spindle);
  else
    START_SPINDLE_COUNTERCLOCKWISE(spindle);

  return INTERP_OK;
}

/****************************************************************************/

/*! convert_cycle_g88

Valor retornado: int
   Si el husillo no gira en sentido horario o antihorario, esto
   devuelve NCE_SPINDLE_NOT_TURNING_IN_G88.
   De lo contrario, devuelve INTERP_OK.

Efectos: ver a continuacion

Llamado por:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

Para el plano XY, esto implementa el siguiente ciclo RS274/NGC,
el cual suele ser de mandrinado:
1. Mover solo el eje z a velocidad de avance actual al valor z especificado.
2. Permanecer el número de segundos dado.
3. Detener el giro del husillo.
4. Detener el programa para que el operador pueda retraer el husillo manualmente.
5. Reiniciar el husillo.

CYCLE_MACRO ha posicionado la herramienta en (x, y, r, a, b, c) cuando esto comienza.

Para los planos XZ e YZ, esto hace movimientos análogos.

*/

int Interp::convert_cycle_g88(block_pointer block,
                              CANON_PLANE plane, //! <Plano seleccionado
                              double x, //! <valor-x donde se ejecuta el ciclo
                              double y, //! <valor de y donde se ejecuta el ciclo
                              double bottom_z, //! <valor de z en la parte inferior del ciclo
                              double dwell, //! <tiempo de espera
                              CANON_DIRECTION direction, //! <dirección de giro al principio
                              int spindle) // husillo en uso
{
  CHKS(((direction != CANON_CLOCKWISE) &&
       (direction != CANON_COUNTERCLOCKWISE)),
      NCE_SPINDLE_NOT_TURNING_IN_G88);

  cycle_feed(block, plane, x, y, bottom_z);
  DWELL(dwell);
  STOP_SPINDLE_TURNING(spindle);
  PROGRAM_STOP();               /* el operador retrae el husillo aquí */
  if (direction == CANON_CLOCKWISE)
    START_SPINDLE_CLOCKWISE(spindle);
  else
    START_SPINDLE_COUNTERCLOCKWISE(spindle);

  return INTERP_OK;
}

/****************************************************************************/

/*! convert_cycle_g89

Valor retornado: int (INTERP_OK)

Efectos: ver a continuacion

Llamado por:
   convert_cycle_xy
   convert_cycle_yz
   convert_cycle_zx

Esto implementa el siguiente ciclo RS274/NGC, destinado a mandrinar:
1. Mover solo el eje z a la velocidad de avance actual al valor z especificado.
2. Permanecer el número de segundos dado.
3. Retraer el eje z a la velocidad de avance actual a clear_z.

CYCLE_MACRO ha posicionado la herramienta en (x, y, r, a, b, c) cuando esto comienza.

Para los planos XZ e YZ, esto hace movimientos análogos.

*/

int Interp::convert_cycle_g89(block_pointer block,
                              CANON_PLANE plane, //! <Plano seleccionado
                              double x, //! <valor-x donde se ejecuta el ciclo
                              double y, //! <valor de y donde se ejecuta el ciclo
                              double clear_z, //! <valor z del plano de despeje
                              double bottom_z, //! <valor de z en la parte inferior del ciclo
                              double dwell) //! <tiempo de permanencia                     
{
  cycle_feed(block, plane, x, y, bottom_z);
  DWELL(dwell);
  cycle_feed(block, plane, x, y, clear_z);

  return INTERP_OK;
}

static const char* plane_name(CANON_PLANE p) {
  switch(p) {
    case CANON_PLANE_XY: return "XY";
    case CANON_PLANE_YZ: return "YZ";
    case CANON_PLANE_XZ: return "XZ";
    case CANON_PLANE_UV: return "UV";
    case CANON_PLANE_VW: return "VW";
    case CANON_PLANE_UW: return "UW";
    default:             return "invalid";
  }
}

/****************************************************************************/

/*! convert_cycle

Valor retornado: int
   Si alguna de las funciones específicas llamadas devuelve un código de error,
   esto devuelve ese código.
   Si se produce alguno de los siguientes errores, esto devuelve el código de error que se muestra.
   De lo contrario, devuelve INTERP_OK.
   1. El valor r no se da la primera vez que se llama a este código después de que
      algún otro modo de movimiento haya estado en efecto:
      NCE_R_CLEARANCE_PLANE_UNSPECIFIED_IN_CYCLE
   2. El número l es cero: NCE_CANNOT_DO_ZERO_REPEATS_OF_CYCLE
   3. El plano seleccionado actualmente no es XY, YZ o XZ.
      NCE_BUG_PLANE_NOT_XY_YZ_OR_XZ

Efectos secundarios:
   Se realizan varios movimientos para ejecutar un ciclo fijo.
   La posición actual se restablece. Los valores de los atributos del ciclo en la configuración
   se puede restablecer

Llamado por: convert_motion

Esta función realiza un par de verificaciones y luego llama a una de tres
funciones, según el plano seleccionado actualmente.

Consulte la documentación de convert_cycle_xy para obtener la mayoría de los detalles.

*/

int Interp::convert_cycle(int motion, //! <un código g; entre G_81 y G_89, un ciclo fijo
                         block_pointer block, //! <puntero a un bloque de instrucciones RS274
                         setup_pointer settings) //! <puntero a la configuración de la máquina          
{
  CANON_PLANE plane;

  CHKS((settings->feed_rate == 0.0), _("Cannot feed with zero feed rate"));
  CHKS((settings->feed_mode == INVERSE_TIME), _("Cannot use inverse time feed with canned cycles"));
  CHKS((settings->cutter_comp_side), _("Cannot use canned cycles with cutter compensation on"));

  plane = settings->plane;
  if (!block->r_flag) {
    if (settings->motion_mode == motion)
      block->r_number = settings->cycle_r;
    else
      ERS(NCE_R_CLEARANCE_PLANE_UNSPECIFIED_IN_CYCLE);
  }

  CHKS((block->l_number == 0), NCE_CANNOT_DO_ZERO_REPEATS_OF_CYCLE);
  if (block->l_number == -1)
    block->l_number = 1;

  switch(plane) {
  case CANON_PLANE_XY:
  case CANON_PLANE_XZ:
  case CANON_PLANE_YZ:
    CHKS(block->u_flag, "Cannot put a U in a canned cycle in the %s plane",
	plane_name(settings->plane));
    CHKS(block->v_flag, "Cannot put a V in a canned cycle in the %s plane",
	plane_name(settings->plane));
    CHKS(block->w_flag, "Cannot put a W in a canned cycle in the %s plane",
	plane_name(settings->plane));
    break;

  case CANON_PLANE_UV:
  case CANON_PLANE_VW:
  case CANON_PLANE_UW:
    CHKS(block->x_flag, "Cannot put an X in a canned cycle in the %s plane",
	plane_name(settings->plane));
    CHKS(block->y_flag, "Cannot put a Y in a canned cycle in the %s plane",
	plane_name(settings->plane));
    CHKS(block->z_flag, "Cannot put a Z in a canned cycle in the %s plane",
	plane_name(settings->plane));
  }

  if (plane == CANON_PLANE_XY) {
    CHP(convert_cycle_xy(motion, block, settings));
  } else if (plane == CANON_PLANE_YZ) {
    CHP(convert_cycle_yz(motion, block, settings));
  } else if (plane == CANON_PLANE_XZ) {
    CHP(convert_cycle_zx(motion, block, settings));
  } else if (plane == CANON_PLANE_UV) {
    CHP(convert_cycle_uv(motion, block, settings));
  } else if (plane == CANON_PLANE_VW) {
    CHP(convert_cycle_vw(motion, block, settings));
  } else if (plane == CANON_PLANE_UW) {
    CHP(convert_cycle_wu(motion, block, settings));
  } else
    ERS(NCE_BUG_PLANE_NOT_XY_YZ_OR_XZ);

  settings->cycle_l = block->l_number;
  settings->cycle_r = block->r_number;
  settings->motion_mode = motion;
  return INTERP_OK;
}

/****************************************************************************/

/*! convert_cycle_xy

Valor retornado: int
   Si alguna de las funciones específicas llamadas devuelve un código de error,
   esto devuelve ese código.
   Si se produce alguno de los siguientes errores, esto devuelve el código de error que se muestra.
   De lo contrario, devuelve INTERP_OK.
   1. El valor z no se da la primera vez que se llama a este código después de que
      algún otro modo de movimiento haya estado en efecto:
      NCE_Z_VALUE_UNSPECIFIED_IN_XY_PLANE_CANNED_CYCLE
   2. El plano libre r está debajo de bottom_z:
      NCE_R_LESS_THAN_Z_IN_CYCLE_IN_XY_PLANE
   3. el modo de distancia no es absoluto ni incremental:
      NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91
   4. Se llama a G82, G86, G88 o G89 cuando aún no está vigente,
      y no hay número p en el bloque:
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89
   5. Se llama a G83 cuando aún no está en vigor,
      y no hay ningún número q en el bloque:
      NCE_Q_WORD_MISSING_WITH_G83_OR_M66
   6. Se llama a G87 cuando aún no está en vigor,
      y falta alguno de los números i, j o ​​k:
      NCE_I_WORD_MISSING_WITH_G87
      NCE_J_WORD_MISSING_WITH_G87
      NCE_K_WORD_MISSING_WITH_G87
   7. el código G no está entre G_81 y G_89.
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED

Efectos secundarios:
   Se realizan varios movimientos para ejecutar el código g

Llamado por: convert_cycle

La función no requiere que x, y, z o r se especifique en
el bloque, excepto si el último comando de modo de movimiento ejecutado
no es lo mismo que este; entonces se deben especificar el valor r y el valor z.

Esta función maneja la caracteristica de repetición de RS274/NGC, en donde
la palabra L representa el número de repeticiones [NCMS, página 99]. No se esta
permitiendo L = 0, contrariamente al manual. Se esta permitiendo L > 1
en modo de distancia absoluta que significa "hacer lo mismo en el mismo
lugar varias veces ", como se indica en el manual, aunque esto parece
anormal.

En el modo de distancia incremental, los valores x, y, y r se tratan como
incrementos de la posición actual y z como un incremento de r. En
modo de distancia absoluta, x, y, r y z son absolutos. En g87, i y j
siempre seran incremento, independientemente de la configuración del modo de distancia, como
se explicita en [NCMS, página 98], pero k (valor z de la parte superior de un cajeado trasero)
sera un valor z absoluto en modo de distancia absoluta, y un incremento
(desde la parte inferior z) en modo de distancia incremental.

Si la posición r de un ciclo está por encima de la posición current_z, esto
retrae el eje z a la posición r antes de moverse en paralelo al
plano XY.

En el código para esta función, hay un bucle "for" casi idéntico
en todos los casos del switch. El bucle se realiza con una
macro "CYCLE_MACRO" para que el código sea fácil de leer, automáticamente
mantenido idéntico de un caso a otro y mucho más corto de lo que sería
sin la macro. El bucle podría colocarse fuera del switch, pero
entonces el conmutador se ejecutará cada vez que se repita el ciclo, no solo una vez,
como lo hace aquí. El bucle también se puede colocar en las funciones
llamadas, pero entonces no estaría claro que todos los bucles son
iguales, y sería difícil mantenerlos iguales cuando el código se
modifica. La macro sería muy incómoda como una función regular
porque tendría que pasar todos los argumentos utilizados por cualquiera de
los ciclos específicos, y, si otro switch en la función va a ser
evitado, tendría que pasar un puntero de función, pero diferentes
funciones de ciclo tienen diferentes argumentos por lo que el tipo de puntero
no podría declararse a menos que las funciones del ciclo se volvieran a escribir para
tomar los mismos argumentos (en cuyo caso la mayoría de ellos tendrían varios
argumentos no utilizados).

Los movimientos dentro de CYCLE_MACRO (pero fuera de un ciclo específico) son
un recorrido recto paralelo al plano seleccionado al dado
a una posición en el plano y un recorrido recto del tercer eje solamente
(si es necesario) a la posición r.

CYCLE_MACRO se define aquí pero también se usa en convert_cycle_yz
y convert_cycle_zx. Las variables aa, bb y cc se utilizan en
CYCLE_MACRO y en las otras dos funciones que acabamos de mencionar. Aquellas
variables representan el primer eje del plano seleccionado, el segundo
eje del plano seleccionado y tercer eje que es perpendicular al
plano seleccionado. En esta función, aa representa x, bb representa
y, y cc representa z. Este uso hace posible tener solo una
versión de cada una de las funciones del ciclo. Cycle_traverse y
las funciones cycle_feed ayudan a lograr esto.

La altura del movimiento de retracción al final de cada repetición de un ciclo es
determinada por la configuración de retract_mode: ya sea a la posicion r
(si retract_mode es R_PLANE) o a la posicion original z
(si está por encima de la posición r y retract_mode no es
R_PLANE). Esta es una ligera desviación de [NCMS, página 98], que
no requiere verificar que la posición z original esté por encima de r.

Los ejes rotativos no pueden moverse durante un ciclo fijo.

*/

int Interp::convert_cycle_xy(int motion, //! <un código g entre G_81 y G_89, un ciclo fijo
                             block_pointer block, //! <puntero a un bloque de instrucciones RS274
                             setup_pointer settings) //! <puntero a la configuración de la máquina                 
{
  double aa;
  double aa_increment=0.;
  double bb;
  double bb_increment=0.;
  double cc;
  double clear_cc;
  double i;
  double j;
  double k;
  double old_cc;
  double radius_increment = 0.;
  double theta_increment = 0.;
  CANON_PLANE plane;
  double r;
  int repeat;
  CANON_MOTION_MODE save_mode;
  double save_tolerance;
  double current_cc = settings->current_z;

  plane = CANON_PLANE_XY;
  if (settings->motion_mode != motion) {
    CHKS((!block->z_flag),
        _readers[(int)'z']? NCE_Z_VALUE_UNSPECIFIED_IN_XY_PLANE_CANNED_CYCLE: _("G17 canned cycle is not possible on a machine without Z axis"));
  }
  block->z_number =
    block->z_flag ? block->z_number : settings->cycle_cc;
  if(settings->cycle_il_flag) {
      old_cc = settings->cycle_il;
  } else {
      old_cc = settings->cycle_il = current_cc;
      settings->cycle_il_flag = true;
  }

  if (settings->distance_mode == MODE_ABSOLUTE) {
    double radius, theta;
    aa_increment = 0.0;
    bb_increment = 0.0;
    r = block->r_number;
    cc = block->z_number;
    if(block->radius_flag)
        radius = block->radius;
    else
        radius = hypot(settings->current_y, settings->current_x);
    if(block->theta_flag)
        theta = D2R(block->theta);
    else
        theta = atan2(settings->current_y, settings->current_x);
    if(block->radius_flag || block->theta_flag) {
        aa = radius * cos(theta);
        bb = radius * sin(theta);
    } else {
        aa = block->x_flag ? block->x_number : settings->current_x;
        bb = block->y_flag ? block->y_number : settings->current_y;
    }
  } else if (settings->distance_mode == MODE_INCREMENTAL) {
    if (block->x_flag) aa_increment = block->x_number;
    if (block->y_flag) bb_increment = block->y_number;
    if (block->radius_flag) radius_increment = block->radius;
    if (block->theta_flag) theta_increment = D2R(block->theta);
    r = (block->r_number + old_cc);
    cc = (r + block->z_number); /* [NCMS, page 98] */
    aa = settings->current_x;
    bb = settings->current_y;
  } else
    ERS(NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91);
  CHKS((r < cc), NCE_R_LESS_THAN_Z_IN_CYCLE_IN_XY_PLANE);

  // Primer movimiento de un ciclo fijo (tal vez): si estamos por debajo del plano R,
  // rápido hasta el plano R.
  if (old_cc < r) {
    STRAIGHT_TRAVERSE(block->line_number, settings->current_x, settings->current_y, r,
                      settings->AA_current, settings->BB_current, settings->CC_current, 
                      settings->u_current, settings->v_current, settings->w_current);
    old_cc = r;
    current_cc = old_cc;
  }
  clear_cc = (settings->retract_mode == R_PLANE) ? r : old_cc;

  save_mode = GET_EXTERNAL_MOTION_CONTROL_MODE();
  save_tolerance = GET_EXTERNAL_MOTION_CONTROL_TOLERANCE();
  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(CANON_EXACT_PATH, 0);

  switch (motion) {
  case G_81:
      CYCLE_MACRO(convert_cycle_g81(block, CANON_PLANE_XY, aa, bb, clear_cc, cc))
      break;
  case G_82:
    CHKS(((settings->motion_mode != G_82) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g82(block, CANON_PLANE_XY, aa, bb, clear_cc, cc,
                                  block->p_number))
      settings->cycle_p = block->p_number;
    break;
  case G_73:
    CHKS(((settings->motion_mode != G_73) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G73);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g73(block, CANON_PLANE_XY, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_83:
    CHKS(((settings->motion_mode != G_83) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G83);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g83(block, CANON_PLANE_XY, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_74:
  case G_84:
      block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
      if (block->dollar_flag){
        CHKS((block->dollar_number < 0 || block->dollar_number >= settings->num_spindles),
            (_("Invalid spindle ($) number in G74/G84 cycle")));
        settings->active_spindle = (int)block->dollar_number;
      }
      CYCLE_MACRO(convert_cycle_g74_g84(block, CANON_PLANE_XY, aa, bb, clear_cc, cc,
                                       settings->spindle_turning[settings->active_spindle],
                                       settings->speed_feed_mode,
                                       motion, block->p_number, settings->active_spindle))
      settings->cycle_p = block->p_number;

      break;
  case G_85:
      CYCLE_MACRO(convert_cycle_g85(block, CANON_PLANE_XY, aa, bb, r, clear_cc, cc))
      break;
  case G_86:
    CHKS(((settings->motion_mode != G_86) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g86(block, CANON_PLANE_XY, aa, bb, clear_cc, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle))
    settings->cycle_p = block->p_number;
    break;
  case G_87:
    if (settings->motion_mode != G_87) {
      CHKS((!block->i_flag), NCE_I_WORD_MISSING_WITH_G87);
      CHKS((!block->j_flag), NCE_J_WORD_MISSING_WITH_G87);
      CHKS((!block->k_flag), NCE_K_WORD_MISSING_WITH_G87);
    }
    i = block->i_flag ? block->i_number : settings->cycle_i;
    j = block->j_flag ? block->j_number : settings->cycle_j;
    k = block->k_flag ? block->k_number : settings->cycle_k;
    settings->cycle_i = i;
    settings->cycle_j = j;
    settings->cycle_k = k;
    if (settings->distance_mode == MODE_INCREMENTAL) {
      k = (cc + k);             /* k siempre es absoluto en la función llamada a continuación */
    }
    CYCLE_MACRO(convert_cycle_g87(block, CANON_PLANE_XY, aa, (aa + i), bb,
                                  (bb + j), r, clear_cc, k, cc,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    break;
  case G_88:
    CHKS(((settings->motion_mode != G_88) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g88(block, CANON_PLANE_XY, aa, bb, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle))
    settings->cycle_p = block->p_number;
    break;

  case G_89:
    CHKS(((settings->motion_mode != G_89) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g89(block, CANON_PLANE_XY, aa, bb, clear_cc, cc,
                                  block->p_number))
      settings->cycle_p = block->p_number;
    break;
  default:
    ERS(NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  }
  settings->current_x = aa;     /* CYCLE_MACRO actualiza aa y bb */
  settings->current_y = bb;
  settings->current_z = clear_cc;
  settings->cycle_cc = block->z_number;

  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(save_mode, save_tolerance);

  return INTERP_OK;
}



int Interp::convert_cycle_uv(int motion, //! <un código g entre G_81 y G_89, un ciclo fijo
                            block_pointer block, //! <puntero a un bloque de instrucciones RS274
                            setup_pointer settings) //! <puntero a la configuración de la máquina                   
{
  int spindle = settings->active_spindle;
  double aa;
  double aa_increment=0.;
  double bb;
  double bb_increment=0.;
  double cc;
  double clear_cc;
  double i;
  double j;
  double k;
  double old_cc;
  double radius_increment = 0.;
  double theta_increment = 0.;
  CANON_PLANE plane;
  double r;
  int repeat;
  CANON_MOTION_MODE save_mode;
  double save_tolerance;
  double current_cc = settings->w_current;

  plane = CANON_PLANE_UV;
  if (settings->motion_mode != motion) {
    CHKS((!block->w_flag),
        _readers[(int)'w']? NCE_W_VALUE_UNSPECIFIED_IN_UV_PLANE_CANNED_CYCLE: _("G17.1 canned cycle is not possible on a machine without W axis"));
  }
  block->w_number =
    block->w_flag ? block->w_number : settings->cycle_cc;
  if(settings->cycle_il_flag) {
      old_cc = settings->cycle_il;
  } else {
      old_cc = settings->cycle_il = current_cc;
      settings->cycle_il_flag = true;
  }

  if (settings->distance_mode == MODE_ABSOLUTE) {
    aa_increment = 0.0;
    bb_increment = 0.0;
    r = block->r_number;
    cc = block->w_number;
    aa = block->u_flag ? block->u_number : settings->u_current;
    bb = block->v_flag ? block->v_number : settings->v_current;
  } else if (settings->distance_mode == MODE_INCREMENTAL) {
    if (block->u_flag) aa_increment = block->u_number;
    if (block->v_flag) bb_increment = block->v_number;
    r = (block->r_number + old_cc);
    cc = (r + block->w_number); /* [NCMS, page 98] */
    aa = settings->u_current;
    bb = settings->v_current;
  } else
    ERS(NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91);
  CHKS((r < cc), NCE_R_LESS_THAN_W_IN_CYCLE_IN_UV_PLANE);

  if (old_cc < r) {
    STRAIGHT_TRAVERSE(block->line_number, settings->current_x, settings->current_y, settings->current_z,
                      settings->AA_current, settings->BB_current, settings->CC_current, 
                      settings->u_current, settings->v_current, r);
    old_cc = r;
    current_cc = old_cc;
  }
  clear_cc = (settings->retract_mode == R_PLANE) ? r : old_cc;

  save_mode = GET_EXTERNAL_MOTION_CONTROL_MODE();
  save_tolerance = GET_EXTERNAL_MOTION_CONTROL_TOLERANCE();
  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(CANON_EXACT_PATH, 0);

  switch (motion) {
  case G_81:
      CYCLE_MACRO(convert_cycle_g81(block, CANON_PLANE_UV, aa, bb, clear_cc, cc))
      break;
  case G_82:
    CHKS(((settings->motion_mode != G_82) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g82(block, CANON_PLANE_UV, aa, bb, clear_cc, cc,
                                  block->p_number))
      settings->cycle_p = block->p_number;
    break;
  case G_83:
    CHKS(((settings->motion_mode != G_83) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G83);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g83(block, CANON_PLANE_UV, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_73:
    CHKS(((settings->motion_mode != G_73) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G73);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g73(block, CANON_PLANE_UV, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_74:
  case G_84:
    block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    if (block->dollar_flag){
        CHKS((block->dollar_number < 0 || block->dollar_number >= settings->num_spindles),
            (_("Invalid spindle ($) numberin G74/G84 cycle")));
        settings->active_spindle = (int)block->dollar_number;
    }
    CYCLE_MACRO(convert_cycle_g74_g84(block, CANON_PLANE_UV, aa, bb, clear_cc, cc,
                                       settings->spindle_turning[spindle],
                                       settings->speed_feed_mode,
                                       motion, block->p_number, settings->active_spindle))
         settings->cycle_p = block->p_number;
    break;
  case G_85:
    CYCLE_MACRO(convert_cycle_g85(block, CANON_PLANE_UV, aa, bb, r, clear_cc, cc))
      break;
  case G_86:
    CHKS(((settings->motion_mode != G_86) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g86(block, CANON_PLANE_UV, aa, bb, clear_cc, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle))
    settings->cycle_p = block->p_number;
    break;
  case G_87:
    if (settings->motion_mode != G_87) {
      CHKS((!block->i_flag), NCE_I_WORD_MISSING_WITH_G87);
      CHKS((!block->j_flag), NCE_J_WORD_MISSING_WITH_G87);
      CHKS((!block->k_flag), NCE_K_WORD_MISSING_WITH_G87);
    }
    i = block->i_flag ? block->i_number : settings->cycle_i;
    j = block->j_flag ? block->j_number : settings->cycle_j;
    k = block->k_flag ? block->k_number : settings->cycle_k;
    settings->cycle_i = i;
    settings->cycle_j = j;
    settings->cycle_k = k;
    if (settings->distance_mode == MODE_INCREMENTAL) {
      k = (cc + k);             /* k siempre es absoluto en la función llamada a continuación */
    }
    CYCLE_MACRO(convert_cycle_g87(block, CANON_PLANE_UV, aa, (aa + i), bb,
                                  (bb + j), r, clear_cc, k, cc,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle))
    break;
  case G_88:
    CHKS(((settings->motion_mode != G_88) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g88(block, CANON_PLANE_UV, aa, bb, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle))
    settings->cycle_p = block->p_number;
    break;
  case G_89:
    CHKS(((settings->motion_mode != G_89) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g89(block, CANON_PLANE_UV, aa, bb, clear_cc, cc,
                                  block->p_number))
    settings->cycle_p = block->p_number;
    break;
  default:
    ERS(NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  }
  settings->u_current = aa;     /* CYCLE_MACRO actualiza aa y bb */
  settings->v_current = bb;
  settings->w_current = clear_cc;
  settings->cycle_cc = block->w_number;

  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(save_mode, save_tolerance);

  return INTERP_OK;
}

/****************************************************************************/

/*! convert_cycle_yz

Valor retornado: int
   Si alguna de las funciones específicas llamadas devuelve un código de error,
   esto devuelve ese código.
   Si se produce alguno de los siguientes errores, esto devuelve el código de error que se muestra.
   De lo contrario, devuelve INTERP_OK.
   1. El valor x no se da la primera vez que se llama a este código después de que
      algún otro modo de movimiento ha estado en efecto:
      NCE_X_VALUE_UNSPECIFIED_IN_YZ_PLANE_CANNED_CYCLE
   2. El plano libre r está debajo de bottom_x:
      NCE_R_LESS_THAN_X_IN_CYCLE_IN_YZ_PLANE
   3. el modo de distancia no es absoluto ni incremental:
      NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91
   4. Se llama a G82, G86, G88 o G89 cuando aún no está vigente,
      y no hay número p en el bloque:
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89
   5. Se llama a G83 cuando aún no está en vigor,
      y no hay ningún número q en el bloque: 
      NCE_Q_WORD_MISSING_WITH_G83
   6. Se llama a G87 cuando aún no está en vigor,
      y falta alguno de los números i, j o ​​k:
      NCE_I_WORD_MISSING_WITH_G87
      NCE_J_WORD_MISSING_WITH_G87
      NCE_K_WORD_MISSING_WITH_G87
   7. el código G no está entre G_81 y G_89.
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED

Efectos secundarios:
   Se realizan varios movimientos para ejecutar un ciclo fijo.

Llamado por: convert_cycle

Consulte la documentación de convert_cycle_xy. Esta función es completamente
similar. En esta función, aa representa y, bb representa z y cc
representa x.

CYCLE_MACRO se define justo antes de la función convert_cycle_xy.

Los desplazamientos de la longitud de la herramienta solo funcionan cuando el eje de la herramienta es paralelo al
eje Z, por lo que si se utiliza esta función, los desplazamientos de longitud de la herramienta deben ser
apagados y el código NC escrito para tener en cuenta la longitud de la herramienta.

*/

int Interp::convert_cycle_yz(int motion, //! <un código g entre G_81 y G_89, un ciclo fijo
                            block_pointer block, //! <puntero a un bloque de instrucciones RS274 / NGC
                            setup_pointer settings) //! <puntero a la configuración de la máquina                 
{
  int spindle = settings->active_spindle;
  double aa;
  double aa_increment=0.;
  double bb;
  double bb_increment=0.;
  double cc;
  double clear_cc;
  double i;
  double j;
  double k;
  double old_cc;
  double radius_increment = 0.;
  double theta_increment = 0.;
  CANON_PLANE plane;
  double r;
  int repeat;
  CANON_MOTION_MODE save_mode;
  double save_tolerance; //guarda la tolerancia actual, para restaurarla más tarde
  double current_cc = settings->current_x;

  plane = CANON_PLANE_YZ;
  if (settings->motion_mode != motion) {
    CHKS((!block->x_flag),
        _readers[(int)'x']? NCE_X_VALUE_UNSPECIFIED_IN_YZ_PLANE_CANNED_CYCLE: _("G19 canned cycle is not possible on a machine without X axis"));
  }
  block->x_number =
    block->x_flag ? block->x_number : settings->cycle_cc;
  if(settings->cycle_il_flag) {
      old_cc = settings->cycle_il;
  } else {
      old_cc = settings->cycle_il = current_cc;
      settings->cycle_il_flag = true;
  }

  if (settings->distance_mode == MODE_ABSOLUTE) {
    aa_increment = 0.0;
    bb_increment = 0.0;
    r = block->r_number;
    cc = block->x_number;
    aa = block->y_flag ? block->y_number : settings->current_y;
    bb = block->z_flag ? block->z_number : settings->current_z;
  } else if (settings->distance_mode == MODE_INCREMENTAL) {
    if (block->y_flag) aa_increment = block->y_number;
    if (block->z_flag) bb_increment = block->z_number;
    r = (block->r_number + old_cc);
    cc = (r + block->x_number); /* [NCMS, page 98] */
    aa = settings->current_y;
    bb = settings->current_z;
  } else
    ERS(NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91);
  CHKS((r < cc), NCE_R_LESS_THAN_X_IN_CYCLE_IN_YZ_PLANE);

  if (old_cc < r) {
    STRAIGHT_TRAVERSE(block->line_number, r, settings->current_y, settings->current_z,
                      settings->AA_current, settings->BB_current, settings->CC_current,
                      settings->u_current, settings->v_current, settings->w_current);
    old_cc = r;
    current_cc = old_cc;
  }
  clear_cc = (settings->retract_mode == R_PLANE) ? r : old_cc;

  save_mode = GET_EXTERNAL_MOTION_CONTROL_MODE();
  save_tolerance = GET_EXTERNAL_MOTION_CONTROL_TOLERANCE();
  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(CANON_EXACT_PATH, 0);

  switch (motion) {
  case G_81:
    CYCLE_MACRO(convert_cycle_g81(block, CANON_PLANE_YZ, aa, bb, clear_cc, cc))
      break;
  case G_82:
    CHKS(((settings->motion_mode != G_82) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g82(block, CANON_PLANE_YZ, aa, bb, clear_cc, cc,
                                  block->p_number))
      settings->cycle_p = block->p_number;
    break;
  case G_73:
    CHKS(((settings->motion_mode != G_73) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G73);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g73(block, CANON_PLANE_YZ, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_83:
    CHKS(((settings->motion_mode != G_83) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G83);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g83(block, CANON_PLANE_YZ, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_74:
  case G_84:
     block->p_number =
     block->p_number == -1.0 ? settings->cycle_p : block->p_number;
     if (block->dollar_flag){
        CHKS((block->dollar_number < 0 || block->dollar_number >= settings->num_spindles),
            (_("Invalid spindle ($) number in G74/G84 cycle")));
        settings->active_spindle = (int)block->dollar_number;
     }
     CYCLE_MACRO(convert_cycle_g74_g84(block, CANON_PLANE_YZ, aa, bb, clear_cc, cc,
                                    settings->spindle_turning[spindle],
                                    settings->speed_feed_mode,
                                    motion, block->p_number, settings->active_spindle))
     settings->cycle_p = block->p_number;
     break;
  case G_85:
    CYCLE_MACRO(convert_cycle_g85(block, CANON_PLANE_YZ, aa, bb, r, clear_cc, cc))
      break;
  case G_86:
    CHKS(((settings->motion_mode != G_86) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g86(block, CANON_PLANE_YZ, aa, bb, clear_cc, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    settings->cycle_p = block->p_number;
    break;
  case G_87:
    if (settings->motion_mode != G_87) {
      CHKS((!block->i_flag), NCE_I_WORD_MISSING_WITH_G87);
      CHKS((!block->j_flag), NCE_J_WORD_MISSING_WITH_G87);
      CHKS((!block->k_flag), NCE_K_WORD_MISSING_WITH_G87);
    }
    i = block->i_flag ? block->i_number : settings->cycle_i;
    j = block->j_flag ? block->j_number : settings->cycle_j;
    k = block->k_flag ? block->k_number : settings->cycle_k;
    settings->cycle_i = i;
    settings->cycle_j = j;
    settings->cycle_k = k;
    if (settings->distance_mode == MODE_INCREMENTAL) {
      i = (cc + i);             /* i siempre absoluto en la función llamada a continuación */
    }
    CYCLE_MACRO(convert_cycle_g87(block, CANON_PLANE_YZ, aa, (aa + j), bb,
                                  (bb + k), r, clear_cc, i, cc,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    break;
  case G_88:
    CHKS(((settings->motion_mode != G_88) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g88(block, CANON_PLANE_YZ, aa, bb, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    settings->cycle_p = block->p_number;
    break;
  case G_89:
    CHKS(((settings->motion_mode != G_89) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g89(block, CANON_PLANE_YZ, aa, bb, clear_cc, cc,
                                  block->p_number))
      settings->cycle_p = block->p_number;
    break;
  default:
    ERS(NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  }
  settings->current_y = aa;     /* CYCLE_MACRO actualiza aa y bb */
  settings->current_z = bb;
  settings->current_x = clear_cc;
  settings->cycle_cc = block->x_number;

  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(save_mode, save_tolerance);

  return INTERP_OK;
}


int Interp::convert_cycle_vw(int motion, //! <un código g entre G_81 y G_89, un ciclo fijo
                            block_pointer block, //! <puntero a un bloque de instrucciones RS274 / NGC
                            setup_pointer settings) //! <puntero a la configuración de la máquina                  
{
  double aa;
  double aa_increment=0.;
  double bb;
  double bb_increment=0.;
  double cc;
  double clear_cc;
  double i;
  double j;
  double k;
  double old_cc;
  double radius_increment = 0.;
  double theta_increment = 0.;
  CANON_PLANE plane;
  double r;
  int repeat;
  CANON_MOTION_MODE save_mode;
  double save_tolerance; //guarda la tolerancia actual, para restaurarla más tarde
  double current_cc = settings->u_current;

  plane = CANON_PLANE_VW;
  if (settings->motion_mode != motion) {
    CHKS((!block->u_flag),
        _readers[(int)'u']? NCE_U_VALUE_UNSPECIFIED_IN_VW_PLANE_CANNED_CYCLE: _("G19.1 canned cycle is not possible on a machine without U axis"));
  }
  block->u_number =
    block->u_flag ? block->u_number : settings->cycle_cc;
  if(settings->cycle_il_flag) {
      old_cc = settings->cycle_il;
  } else {
      old_cc = settings->cycle_il = current_cc;
      settings->cycle_il_flag = true;
  }

  if (settings->distance_mode == MODE_ABSOLUTE) {
    aa_increment = 0.0;
    bb_increment = 0.0;
    r = block->r_number;
    cc = block->u_number;
    aa = block->v_flag ? block->v_number : settings->v_current;
    bb = block->w_flag ? block->w_number : settings->w_current;
  } else if (settings->distance_mode == MODE_INCREMENTAL) {
    if (block->v_flag) aa_increment = block->v_number;
    if (block->w_flag) bb_increment = block->w_number;
    r = (block->r_number + old_cc);
    cc = (r + block->u_number); /* [NCMS, page 98] */
    aa = settings->v_current;
    bb = settings->w_current;
  } else
    ERS(NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91);
  CHKS((r < cc), NCE_R_LESS_THAN_U_IN_CYCLE_IN_VW_PLANE);

  if (old_cc < r) {
    STRAIGHT_TRAVERSE(block->line_number, settings->current_x, settings->current_y, settings->current_z,
                      settings->AA_current, settings->BB_current, settings->CC_current,
                      r, settings->v_current, settings->w_current);
    old_cc = r;
    current_cc = old_cc;
  }
  clear_cc = (settings->retract_mode == R_PLANE) ? r : old_cc;

  save_mode = GET_EXTERNAL_MOTION_CONTROL_MODE();
  save_tolerance = GET_EXTERNAL_MOTION_CONTROL_TOLERANCE();
  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(CANON_EXACT_PATH, 0);

  switch (motion) {
  case G_81:
    CYCLE_MACRO(convert_cycle_g81(block, CANON_PLANE_VW, aa, bb, clear_cc, cc))
      break;
  case G_82:
    CHKS(((settings->motion_mode != G_82) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g82(block, CANON_PLANE_VW, aa, bb, clear_cc, cc,
                                  block->p_number))
      settings->cycle_p = block->p_number;
    break;
  case G_83:
    CHKS(((settings->motion_mode != G_83) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G83);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g83(block, CANON_PLANE_VW, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_73:
    CHKS(((settings->motion_mode != G_73) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G73);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g73(block, CANON_PLANE_VW, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_74:
  case G_84:
    block->p_number =
    block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    if (block->dollar_flag){
        CHKS((block->dollar_number < 0 || block->dollar_number >= settings->num_spindles),
            (_("Invalid spindle ($) number in G74/G84 cycle")));
        settings->active_spindle = (int)block->dollar_number;
    }
    CYCLE_MACRO(convert_cycle_g74_g84(block, CANON_PLANE_VW, aa, bb, clear_cc, cc,
                                     settings->spindle_turning[settings->active_spindle],
                                     settings->speed_feed_mode,
                                     motion, block->p_number, settings->active_spindle))
    settings->cycle_p = block->p_number;
    break;
  case G_85:
    CYCLE_MACRO(convert_cycle_g85(block, CANON_PLANE_VW, aa, bb, r, clear_cc, cc))
      break;
  case G_86:
    CHKS(((settings->motion_mode != G_86) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g86(block, CANON_PLANE_VW, aa, bb, clear_cc, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    settings->cycle_p = block->p_number;
    break;
  case G_87:
    if (settings->motion_mode != G_87) {
      CHKS((!block->i_flag), NCE_I_WORD_MISSING_WITH_G87);
      CHKS((!block->j_flag), NCE_J_WORD_MISSING_WITH_G87);
      CHKS((!block->k_flag), NCE_K_WORD_MISSING_WITH_G87);
    }
    i = block->i_flag ? block->i_number : settings->cycle_i;
    j = block->j_flag ? block->j_number : settings->cycle_j;
    k = block->k_flag ? block->k_number : settings->cycle_k;
    settings->cycle_i = i;
    settings->cycle_j = j;
    settings->cycle_k = k;
    if (settings->distance_mode == MODE_INCREMENTAL) {
      i = (cc + i);             /* i siempre absoluto en la función llamada a continuación */
    }
    CYCLE_MACRO(convert_cycle_g87(block, CANON_PLANE_VW, aa, (aa + j), bb,
                                  (bb + k), r, clear_cc, i, cc,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    break;
  case G_88:
    CHKS(((settings->motion_mode != G_88) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g88(block, CANON_PLANE_VW, aa, bb, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    settings->cycle_p = block->p_number;
    break;
  case G_89:
    CHKS(((settings->motion_mode != G_89) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g89(block, CANON_PLANE_VW, aa, bb, clear_cc, cc,
                                  block->p_number))
      settings->cycle_p = block->p_number;
    break;
  default:
    ERS(NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  }
  settings->v_current = aa;     /* CYCLE_MACRO actualiza aa y bb */
  settings->w_current = bb;
  settings->u_current = clear_cc;
  settings->cycle_cc = block->u_number;

  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(save_mode, save_tolerance);

  return INTERP_OK;
}


/****************************************************************************/

/*! convert_cycle_zx

Valor retornado: int
   Si alguna de las funciones específicas llamadas devuelve un código de error,
   esto devuelve ese código.
   Si se produce alguno de los siguientes errores, esto devuelve el código de ERROR que se muestra.
   De lo contrario, devuelve INTERP_OK.
   1. El valor y no se da la primera vez que se llama a este código después de que
      algún otro modo de movimiento ha estado en efecto:
      NCE_Y_VALUE_UNSPECIFIED_IN_XZ_PLANE_CANNED_CYCLE
   2. El plano libre r está debajo de bottom_y:
      NCE_R_LESS_THAN_Y_IN_CYCLE_IN_XZ_PLANE
   3. el modo de distancia no es absoluto ni incremental:
      NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91
   4. Se llama a G82, G86, G88 o G89 cuando aún no está vigente,
      y no hay número p en el bloque:
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88
      NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89
   5. Se llama a G83 cuando aún no está en vigor,
      y no hay ningún número q en el bloque: 
      NCE_Q_WORD_MISSING_WITH_G83
   6. Se llama a G87 cuando aún no está en vigor,
      y falta alguno de los números i, j o ​​k:
      NCE_I_WORD_MISSING_WITH_G87
      NCE_J_WORD_MISSING_WITH_G87
      NCE_K_WORD_MISSING_WITH_G87
   7. el código G no está entre G_81 y G_89.
      NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED

Efectos secundarios:
   Se realizan varios movimientos para ejecutar un ciclo fijo.

Llamado por: convert_cycle

Consulte la documentación de convert_cycle_xy. Esta función es completamente
similar. En esta función, aa representa z, bb representa x y cc
representa y.

CYCLE_MACRO se define justo antes de la función convert_cycle_xy.

Los desplazamientos de la longitud de la herramienta solo funcionan cuando el eje de la herramienta es paralelo al
eje Z, por lo que si se utiliza esta función, los desplazamientos de longitud de la herramienta deben ser
apagados y el código NC escrito para tener en cuenta la longitud de la herramienta.

Es un poco molesto que esta función use zx en algunos lugares
y xz en otros; el uso uniforme de zx sería bueno, ya que ese es el
orden para un sistema de coordenadas a derechas. También con ese uso,
la permutación de los símbolos x, y y z permitiría automáticamente
convirtiendo la función convert_cycle_xy (o convert_cycle_yz) en
la función convert_cycle_xz. Sin embargo, la interfaz canónica usa
CANON_PLANE_XZ.


*/

int Interp::convert_cycle_zx(int motion, //! <un código g entre G_81 y G_89, un ciclo fijo
                            block_pointer block, //! <puntero a un bloque de instrucciones RS274
                            setup_pointer settings) //! <puntero a la configuración de la máquina                  
{
  double aa;
  double aa_increment=0.;
  double bb;
  double bb_increment=0.;
  double cc;
  double clear_cc;
  double i;
  double j;
  double k;
  double old_cc;
  double radius_increment = 0.;
  double theta_increment = 0.;
  CANON_PLANE plane;
  double r;
  int repeat;
  CANON_MOTION_MODE save_mode;
  double save_tolerance; //guarda la tolerancia de seguimiento de ruta actual para restaurarla más tarde
  double current_cc = settings->current_y;

  plane = CANON_PLANE_XZ;
  if (settings->motion_mode != motion) {
    CHKS((!block->y_flag),
        _readers[(int)'y']? NCE_Y_VALUE_UNSPECIFIED_IN_XZ_PLANE_CANNED_CYCLE: _("G18 canned cycle is not possible on a machine without Y axis"));
  }
  block->y_number =
    block->y_flag ? block->y_number : settings->cycle_cc;
  if(settings->cycle_il_flag) {
      old_cc = settings->cycle_il;
  } else {
      old_cc = settings->cycle_il = current_cc;
      settings->cycle_il_flag = true;
  }

  if (settings->distance_mode == MODE_ABSOLUTE) {
    aa_increment = 0.0;
    bb_increment = 0.0;
    r = block->r_number;
    cc = block->y_number;
    aa = block->z_flag ? block->z_number : settings->current_z;
    bb = block->x_flag ? block->x_number : settings->current_x;
  } else if (settings->distance_mode == MODE_INCREMENTAL) {
    if (block->z_flag) aa_increment = block->z_number;
    if (block->x_flag) bb_increment = block->x_number;
    r = (block->r_number + old_cc);
    cc = (r + block->y_number); /* [NCMS, page 98] */
    aa = settings->current_z;
    bb = settings->current_x;
  } else
    ERS(NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91);
  CHKS((r < cc), NCE_R_LESS_THAN_Y_IN_CYCLE_IN_XZ_PLANE);

  if (old_cc < r) {
    STRAIGHT_TRAVERSE(block->line_number, settings->current_x, r, settings->current_z,
                      settings->AA_current, settings->BB_current, settings->CC_current,
                      settings->u_current, settings->v_current, settings->w_current);
    old_cc = r;
    current_cc = old_cc;
  }
  clear_cc = (settings->retract_mode == R_PLANE) ? r : old_cc;

  save_mode = GET_EXTERNAL_MOTION_CONTROL_MODE();
  save_tolerance = GET_EXTERNAL_MOTION_CONTROL_TOLERANCE();
  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(CANON_EXACT_PATH, 0);

  switch (motion) {
  case G_81:
    CYCLE_MACRO(convert_cycle_g81(block, CANON_PLANE_XZ, aa, bb, clear_cc, cc))
      break;
  case G_82:
    CHKS(((settings->motion_mode != G_82) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g82(block, CANON_PLANE_XZ, aa, bb, clear_cc, cc,
                                  block->p_number))
      settings->cycle_p = block->p_number;
    break;
  case G_73:
    CHKS(((settings->motion_mode != G_73) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G73);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g73(block, CANON_PLANE_XZ, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_83:
    CHKS(((settings->motion_mode != G_83) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G83);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g83(block, CANON_PLANE_XZ, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_74:
  case G_84:
     block->p_number =
         block->p_number == -1.0 ? settings->cycle_p : block->p_number;
     if (block->dollar_flag){
        CHKS((block->dollar_number < 0 || block->dollar_number >= settings->num_spindles),
            (_("Invalid E-number in G74/G84 cycle")));
        settings->active_spindle = (int)block->dollar_number;
     }
    CYCLE_MACRO(convert_cycle_g74_g84(block, CANON_PLANE_XZ, aa, bb, clear_cc, cc,
                                     settings->spindle_turning[settings->active_spindle],
                                     settings->speed_feed_mode,
                                     motion, block->p_number, settings->active_spindle))
       settings->cycle_p = block->p_number;
    break;
  case G_85:
    CYCLE_MACRO(convert_cycle_g85(block, CANON_PLANE_XZ, aa, bb, r, clear_cc, cc));
    break;
  case G_86:
    CHKS(((settings->motion_mode != G_86) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g86(block, CANON_PLANE_XZ, aa, bb, clear_cc, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    settings->cycle_p = block->p_number;
    break;
  case G_87:
    if (settings->motion_mode != G_87) {
      CHKS((!block->i_flag), NCE_I_WORD_MISSING_WITH_G87);
      CHKS((!block->j_flag), NCE_J_WORD_MISSING_WITH_G87);
      CHKS((!block->k_flag), NCE_K_WORD_MISSING_WITH_G87);
    }
    i = block->i_flag ? block->i_number : settings->cycle_i;
    j = block->j_flag ? block->j_number : settings->cycle_j;
    k = block->k_flag ? block->k_number : settings->cycle_k;
    settings->cycle_i = i;
    settings->cycle_j = j;
    settings->cycle_k = k;
    if (settings->distance_mode == MODE_INCREMENTAL) {
      j = (cc + j);             /* j siempre es absoluto en la función llamada a continuación  */
    }
    CYCLE_MACRO(convert_cycle_g87(block, CANON_PLANE_XZ, aa, (aa + k), bb,
                                  (bb + i), r, clear_cc, j, cc,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    break;
  case G_88:
    CHKS(((settings->motion_mode != G_88) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g88(block, CANON_PLANE_XZ, aa, bb, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    settings->cycle_p = block->p_number;
    break;
  case G_89:
    CHKS(((settings->motion_mode != G_89) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g89(block, CANON_PLANE_XZ, aa, bb, clear_cc, cc,
                                  block->p_number))
      settings->cycle_p = block->p_number;
    break;
  default:
    ERS(NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  }
  settings->current_z = aa;     /* CYCLE_MACRO actualiza aa y bb */
  settings->current_x = bb;
  settings->current_y = clear_cc;
  settings->cycle_cc = block->y_number;

  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(save_mode, save_tolerance);

  return INTERP_OK;
}

int Interp::convert_cycle_wu(int motion, //!<un código g entre G_81 y G_89, un ciclo fijo
                            block_pointer block,        //!< puntero a un bloque de instrucciones RS274   
                            setup_pointer settings)     //!< puntero a la configuración de la máquina    
{
  double aa;
  double aa_increment=0.;
  double bb;
  double bb_increment=0.;
  double cc;
  double clear_cc;
  double i;
  double j;
  double k;
  double old_cc;
  double radius_increment = 0.;
  double theta_increment = 0.;
  CANON_PLANE plane;
  double r;
  int repeat;
  CANON_MOTION_MODE save_mode;
  double save_tolerance; //guarda la tolerancia de seguimiento de ruta actual para restaurarla más tarde
  double current_cc = settings->v_current;

  plane = CANON_PLANE_UW;
  if (settings->motion_mode != motion) {
    CHKS((!block->v_flag),
        _readers[(int)'v']? NCE_V_VALUE_UNSPECIFIED_IN_UW_PLANE_CANNED_CYCLE: _("G18.1 canned cycle is not possible on a machine without V axis"));
  }
  block->v_number =
    block->v_flag ? block->v_number : settings->cycle_cc;
  if(settings->cycle_il_flag) {
      old_cc = settings->cycle_il;
  } else {
      old_cc = settings->cycle_il = current_cc;
      settings->cycle_il_flag = true;
  }

  if (settings->distance_mode == MODE_ABSOLUTE) {
    aa_increment = 0.0;
    bb_increment = 0.0;
    r = block->r_number;
    cc = block->v_number;
    aa = block->w_flag ? block->w_number : settings->w_current;
    bb = block->u_flag ? block->u_number : settings->u_current;
  } else if (settings->distance_mode == MODE_INCREMENTAL) {
    if (block->w_flag) aa_increment = block->w_number;
    if (block->u_flag) bb_increment = block->u_number;
    r = (block->r_number + old_cc);
    cc = (r + block->v_number); /* [NCMS, page 98] */
    aa = settings->w_current;
    bb = settings->u_current;
  } else
    ERS(NCE_BUG_DISTANCE_MODE_NOT_G90_OR_G91);
  CHKS((r < cc), NCE_R_LESS_THAN_V_IN_CYCLE_IN_UW_PLANE);

  if (old_cc < r) {
    STRAIGHT_TRAVERSE(block->line_number, settings->current_x, settings->current_y, settings->current_z,
                      settings->AA_current, settings->BB_current, settings->CC_current,
                      settings->u_current, r, settings->w_current);
    old_cc = r;
    current_cc = old_cc;
  }
  clear_cc = (settings->retract_mode == R_PLANE) ? r : old_cc;

  save_mode = GET_EXTERNAL_MOTION_CONTROL_MODE();
  save_tolerance = GET_EXTERNAL_MOTION_CONTROL_TOLERANCE();
  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(CANON_EXACT_PATH, 0);

  switch (motion) {
  case G_81:
    CYCLE_MACRO(convert_cycle_g81(block, CANON_PLANE_UW, aa, bb, clear_cc, cc))
      break;
  case G_82:
    CHKS(((settings->motion_mode != G_82) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G82);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g82(block, CANON_PLANE_UW, aa, bb, clear_cc, cc,
                                  block->p_number))
      settings->cycle_p = block->p_number;
    break;
  case G_73:
    CHKS(((settings->motion_mode != G_73) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G73);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g73(block, CANON_PLANE_UW, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_83:
    CHKS(((settings->motion_mode != G_83) && (block->q_number == -1.0)),
        NCE_Q_WORD_MISSING_WITH_G83);
    block->q_number =
      block->q_number == -1.0 ? settings->cycle_q : block->q_number;
    CYCLE_MACRO(convert_cycle_g83(block, CANON_PLANE_UW, aa, bb, r, clear_cc, cc,
                                  block->q_number))
      settings->cycle_q = block->q_number;
    break;
  case G_74:
  case G_84:
     block->p_number =
     block->p_number == -1.0 ? settings->cycle_p : block->p_number;
     if (block->dollar_flag){
        CHKS((block->dollar_number < 0 || block->dollar_number >= settings->num_spindles),
            (_("Invalid spindle ($) number in G74/G84 cycle")));
        settings->active_spindle = (int)block->dollar_number;
     }
     CYCLE_MACRO(convert_cycle_g74_g84(block, CANON_PLANE_UW, aa, bb, clear_cc, cc,
                                     settings->spindle_turning[settings->active_spindle],
                                     settings->speed_feed_mode,
                                     motion, block->p_number, settings->active_spindle))
      settings->cycle_p = block->p_number;
      break;
  case G_85:
    CYCLE_MACRO(convert_cycle_g85(block, CANON_PLANE_UW, aa, bb, r, clear_cc, cc))
      break;
  case G_86:
    CHKS(((settings->motion_mode != G_86) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G86);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    if (block->dollar_flag){
        CHKS((block->dollar_number < 0 || block->dollar_number >= settings->num_spindles),
            (_("Invalid spindle ($) number in G74/G84 cycle")));
        settings->active_spindle = (int)block->dollar_number;
     }
    CYCLE_MACRO(convert_cycle_g86(block, CANON_PLANE_UW, aa, bb, clear_cc, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    settings->cycle_p = block->p_number;
    break;
  case G_87:
    if (settings->motion_mode != G_87) {
      CHKS((!block->i_flag), NCE_I_WORD_MISSING_WITH_G87);
      CHKS((!block->j_flag), NCE_J_WORD_MISSING_WITH_G87);
      CHKS((!block->k_flag), NCE_K_WORD_MISSING_WITH_G87);
    }
    if (block->dollar_flag){
        CHKS((block->dollar_number < 0 || block->dollar_number >= settings->num_spindles),
            (_("Invalid spindle ($) number in G74/G84 cycle")));
        settings->active_spindle = (int)block->dollar_number;
     }
    i = block->i_flag ? block->i_number : settings->cycle_i;
    j = block->j_flag ? block->j_number : settings->cycle_j;
    k = block->k_flag ? block->k_number : settings->cycle_k;
    settings->cycle_i = i;
    settings->cycle_j = j;
    settings->cycle_k = k;
    if (settings->distance_mode == MODE_INCREMENTAL) {
      j = (cc + j);             /* j siempre es absoluto en la función llamada a continuación  */
    }
    CYCLE_MACRO(convert_cycle_g87(block, CANON_PLANE_UW, aa, (aa + k), bb,
                                  (bb + i), r, clear_cc, j, cc,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    break;
  case G_88:
    CHKS(((settings->motion_mode != G_88) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G88);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    if (block->dollar_flag){
        CHKS((block->dollar_number < 0 || block->dollar_number >= settings->num_spindles),
            (_("Invalid spindle ($) number in G74/G84 cycle")));
        settings->active_spindle = (int)block->dollar_number;
     }
    CYCLE_MACRO(convert_cycle_g88(block, CANON_PLANE_UW, aa, bb, cc,
                                  block->p_number,
                                  settings->spindle_turning[settings->active_spindle],
                                  settings->active_spindle));
    settings->cycle_p = block->p_number;
    break;
  case G_89:
    CHKS(((settings->motion_mode != G_89) && (block->p_number == -1.0)),
        NCE_DWELL_TIME_P_WORD_MISSING_WITH_G89);
    block->p_number =
      block->p_number == -1.0 ? settings->cycle_p : block->p_number;
    CYCLE_MACRO(convert_cycle_g89(block, CANON_PLANE_UW, aa, bb, clear_cc, cc,
                                  block->p_number))
      settings->cycle_p = block->p_number;
    break;
  default:
    ERS(NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);
  }
  settings->w_current = aa;     /* CYCLE_MACRO updates aa and bb */
  settings->u_current = bb;
  settings->v_current = clear_cc;
  settings->cycle_cc = block->v_number;

  if (save_mode != CANON_EXACT_PATH)
    SET_MOTION_CONTROL_MODE(save_mode, save_tolerance);

  return INTERP_OK;
}
/****************************************************************************/

/*! cycle_feed

Valor retornado: int (INTERP_OK)

Efectos secundarios:
  Se llama a STRAIGHT_FEED.

Llamado por:
  convert_cycle_g81
  convert_cycle_g82
  convert_cycle_g83
  convert_cycle_g74_g84
  convert_cycle_g85
  convert_cycle_g86
  convert_cycle_g87
  convert_cycle_g88
  convert_cycle_g89

Esto escribe un comando STRAIGHT_FEED apropiado para un movimiento de ciclo con
respeto al plano dado. No se produce movimiento de ejes rotativos.

*/

int Interp::cycle_feed(block_pointer block,
                       CANON_PLANE plane, //! <Plano seleccionado actualmente
                       double end1, //! <primer valor de coordenadas
                       double end2, //! <segundo valor de coordenadas
                       double end3) //! <tercer valor de coordenadas   
{
    if (plane == CANON_PLANE_XY)
        STRAIGHT_FEED(block->line_number, end1, end2, end3,
                      _setup.AA_current, _setup.BB_current, _setup.CC_current,
                      _setup.u_current, _setup.v_current, _setup.w_current);
    else if (plane == CANON_PLANE_YZ)
        STRAIGHT_FEED(block->line_number, end3, end1, end2,
                      _setup.AA_current, _setup.BB_current, _setup.CC_current,
                      _setup.u_current, _setup.v_current, _setup.w_current);
    else if (plane == CANON_PLANE_XZ)
        STRAIGHT_FEED(block->line_number, end2, end3, end1,
                      _setup.AA_current, _setup.BB_current, _setup.CC_current,
                      _setup.u_current, _setup.v_current, _setup.w_current);
    else if (plane == CANON_PLANE_UV)
        STRAIGHT_FEED(block->line_number, _setup.current_x, _setup.current_y, _setup.current_z,
                      _setup.AA_current, _setup.BB_current, _setup.CC_current,
                      end1, end2, end3);
    else if (plane == CANON_PLANE_VW)
        STRAIGHT_FEED(block->line_number, _setup.current_x, _setup.current_y, _setup.current_z,
                      _setup.AA_current, _setup.BB_current, _setup.CC_current,
                      end3, end1, end2);
    else // (plano == CANON_PLANE_UW)
        STRAIGHT_FEED(block->line_number, _setup.current_x, _setup.current_y, _setup.current_z,
                      _setup.AA_current, _setup.BB_current, _setup.CC_current,
                      end2, end3, end1);
    return INTERP_OK;
}

/****************************************************************************/

/*! cycle_traverse

Valor retornado: int (INTERP_OK)

Efectos secundarios:
  Se llama a STRAIGHT_TRAVERSE.

Llamado por:
  convert_cycle
  convert_cycle_g81
  convert_cycle_g82
  convert_cycle_g83
  convert_cycle_g86
  convert_cycle_g87
  convert_cycle_xy (a través de CYCLE_MACRO)
  convert_cycle_yz (a través de CYCLE_MACRO)
  convert_cycle_zx (a través de CYCLE_MACRO)

Esto escribe un comando STRAIGHT_TRAVERSE apropiado para un ciclo
que se mueve con respecto al plano dado. No se produce movimiento de ejes rotativos.

*/

int Interp::cycle_traverse(block_pointer block,
                           CANON_PLANE plane, //! <Plano seleccionado actualmente
                           double end1, //! <primer valor de coordenadas
                           double end2, //! <segundo valor de coordenadas
                           double end3) //! <tercer valor de coordenadas  
{

    if (plane == CANON_PLANE_XY)
        STRAIGHT_TRAVERSE(block->line_number, end1, end2, end3,
                          _setup.AA_current, _setup.BB_current, _setup.CC_current,
                          _setup.u_current, _setup.v_current, _setup.w_current);
    else if (plane == CANON_PLANE_YZ)
        STRAIGHT_TRAVERSE(block->line_number, end3, end1, end2,
                          _setup.AA_current, _setup.BB_current, _setup.CC_current,
                          _setup.u_current, _setup.v_current, _setup.w_current);
    else if (plane == CANON_PLANE_XZ)
        STRAIGHT_TRAVERSE(block->line_number, end2, end3, end1,
                          _setup.AA_current, _setup.BB_current, _setup.CC_current,
                          _setup.u_current, _setup.v_current, _setup.w_current);
    else if (plane == CANON_PLANE_UV)
        STRAIGHT_TRAVERSE(block->line_number, _setup.current_x, _setup.current_y, _setup.current_z,
                          _setup.AA_current, _setup.BB_current, _setup.CC_current,
                          end1, end2, end3);
    else if (plane == CANON_PLANE_VW)
        STRAIGHT_TRAVERSE(block->line_number, _setup.current_x, _setup.current_y, _setup.current_z,
                          _setup.AA_current, _setup.BB_current, _setup.CC_current,
                          end3, end1, end2);
    else // (plano == CANON_PLANE_UW)
        STRAIGHT_TRAVERSE(block->line_number, _setup.current_x, _setup.current_y, _setup.current_z,
                          _setup.AA_current, _setup.BB_current, _setup.CC_current,
                          end2, end3, end1);
    return INTERP_OK;
}
