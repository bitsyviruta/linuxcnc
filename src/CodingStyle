        


            Introducción

La mayor parte de esta guía de estilo se ha copiado mayormente de documentos 
del kernel de Linux: consulte linux/Documentation/CodingStyle para obtener el texto completo.
Las recomendaciones de C ++ se han extraído de varias fuentes, como los
encabezados de GTK, "C++ The Complete Reference" de Schildt, y discusiones en el IRC.


            Sangría

Justificación: la idea detrás de la sangría es definir claramente dónde
comienza y termina un bloque de control. Especialmente cuando ha estado buscando
en su pantalla durante 20 horas seguidas, le resultará mucho más fácil ver
cómo funciona la sangría si tiene sangrías grandes.

Ahora bien; algunas personas afirman que tener sangrías de 8 caracteres hace
el código se mueva demasiado hacia la derecha y dificulta la lectura en una
pantalla de terminal de 80 caracteres. Mientras, el estilo GNU de 2 caracteres
de sangría reducen la claridad. Para LinuxCNC, se ha elegido un compromiso de 
4 caracteres de sangría. Esto todavía extiende el código y causa
líneas retorcidas que conducen a dificultades para leer las fuentes.
La respuesta a eso es que si necesita más de 3 niveles de sangría, está
retorciendo de todos modos, y debería arreglar su programa. 


            Colocación de llaves

El otro problema que siempre surge en el estilo C es la colocación de
llaves. A diferencia del tamaño de la sangría, hay pocas razones técnicas para
elija una estrategia de colocación sobre la otra, pero la forma preferida, como
nos muestran los profetas Kernighan y Ritchie, es poner la apertura
la última en la línea y colocar la llave de cierre la primera, por lo tanto:

	if (x es true) {
		hacemos y
	}

Sin embargo, hay un caso especial, a saber, las funciones: tienen la
llave de apertura al comienzo de la siguiente línea, por lo tanto:

	int funcion(int x)
	{
		cuerpo de la funcion
	}

Tenga en cuenta que la llave de cierre está vacía en una línea propia, excepto en
los casos en que es seguido por una continuación de la misma declaración,
es decir, un "while" en una declaración do o un "else" en una declaración if, como
esta:

	do {
		cuerpo del lazo
	} while (condicion);

y

	if (x == y) {
		..
	} else if (x > y) {
		...
	} else {
		....
	}

Además, tenga en cuenta que esta colocación de llaves también minimiza el número de 
lineas vacías (o casi vacías), sin pérdida de legibilidad. Por tanto, como el
suministro de nueva-línea en su pantalla no es un recurso renovable (piense en
pantallas de terminal de 25 líneas), tiene más líneas vacías para poner
comentarios.


            Nombrado

C es un lenguaje espartano, y así debería ser su nombrado. A diferencia de Modula-2
y Pascal, los programadores C no usan nombres bonitos como
EstaVariableEsUnContadorTemporal. Un programador de C llamaría esa
variable "tmp", que es mucho más fácil de escribir, y, no menos importante,
difícil de comprender.

SIN EMBARGO, mientras que los nombres con mayúsculas y minúsculas están mal vistos, los nombres descriptivos para
Las variables globales son imprescindibles. Llamar a una función global "foo" es una
ofensa.

Las variables GLOBALES (para usar solo si realmente las necesita) deben
tener nombres descriptivos, al igual que las funciones globales. Si tienes una función
que cuenta el número de usuarios activos, debe llamar a eso
"count_active_users()" o similar; no debería llamarla "cntusr()".

Codificar el tipo de una función en el nombre (llamada notacion Húngara) es poco inteligente - el compilador conoce los tipos de todos modos y puede verifícarlos, y solo confunde al programador. No es de extrañar que Microsoft
haga programas con errores.

Los nombres de variables LOCALES deben ser cortos y al grano. Si usted tiene
algún contador de bucle entero aleatorio, probablemente debería llamarse "i".
Llamarlo "loop_counter" no es productivo, si no hay posibilidad de que 
sea mal entendido. Del mismo modo, "tmp" puede ser casi cualquier tipo de
variable que se utiliza para mantener un valor temporal.

Si tiene miedo de mezclar sus nombres de variables locales, tiene otro
problema, que se llama síndrome de desequilibrio de la hormona de expansion de funciones.
Ver el próximo capítulo.


            Las funciones

Las funciones deben ser cortas y claras, y hacer una sola cosa. Deberían
caber en una o dos pantallas de texto (el tamaño de pantalla ISO/ANSI es 80x24,
como todos sabemos), y hacer una sola cosa y hacerla bien.

La longitud máxima de una función es inversamente proporcional a la
complejidad y nivel de sangría de esa función. Por tanto, si tiene un
función conceptual simple que es solo una larga (pero simple)
declaración-case, donde tienes que hacer cosas pequeñas para muchos
diferentes casos, está bien tener una función más larga.

Sin embargo, si tiene una función compleja y sospecha que un
estudiante de primer año de secundaria no podría ni siquiera
entender de qué se trata la función, deberia descomponerla aun mas
Use las funciones de ayuda con nombres descriptivos (puede pedirle 
al compilador que los incorpore si cree que es crítico para el rendimiento
y probablemente lo hará mejor que usted)

Otra medida de la función es el número de variables locales.
No debe exceder de 5-10, o está haciendo algo mal. Repiense la
función, y dividala en pedazos más pequeños. Un cerebro humano puede,
en general, realizar un seguimiento de aproximadamente 7 cosas diferentes.
Mas de esoalgo y se confunde. Usted sabe que es brillante, pero tal vez le gustaría
entender lo que hizo dentro de 2 semanas.		


            Comentarios

Los comentarios son buenos, pero también existe el peligro de comentar en exceso. NUNCA
intente explicar CÓMO funciona su código en un comentario: es mucho mejor
escribir el código para que el _trabajo_ sea obvio, y es un desperdicio de tiempo
explicar un código mal escrito.

En general, desee que sus comentarios digan QUÉ hace su código, no CÓMO.
Un comentario en recuadro que describe la función, el valor de retorno y quién lo llama,
colocado encima del cuerpo es bueno.
Además, trate de evitar poner comentarios dentro del cuerpo de una función: si la
función es tan compleja que necesita comentar por separado partes de ella,
probablemente debería volver a leer la sección Funciones nuevamente. Puedes hacer
pequeños comentarios para anotar o advertir sobre algo particularmente inteligente (o
feo), pero trate de evitar el exceso. En cambio, ponga los comentarios a la cabeza
de la función,y diga a la gente lo que hace y posiblemente POR QUÉ lo hace.

Si se utilizan comentarios en la línea de /* Fix me */, por favor, por favor, diga
por qué algo necesita arreglarse. Cuando se ha realizado un cambio en
parte del código, elimine el comentario o modifíquelo para indicar que
se ha realizado un cambio y necesita pruebas.

			Scripts de Shell y Makefiles

No todos tienen las mismas herramientas y paquetes instalados. Algunas personas usan
vi, otros emacs: algunos incluso evitan tener instalado cualquier paquete,
prefiriendo un editor de texto ligero como nano o el incorporado en
Midnight Commander

gawk versus mawk: una vez más, no todos tendrán gawk instalado, mawk es
casi una décima parte menor y, sin embargo, se ajusta al estándar Posix AWK. Si
se necesita algún comando oscuro de gawk específico que mawk no proporcione,
el script se romperá para algunos usuarios. Lo mismo se aplicaría a mawk.
En resumen, utilice la invocación genérica awk con preferencia a gawk o mawk.


            Convenciones C ++

Los estilos de codificación de C++ siempre terminan en acalorados debates (un poco como
los argumentos de emacs versus vi). Sin embargo, una cosa es cierta, un 
estilo común utilizado por todos los que trabajan en un proyecto conduce a un código uniforme y legible.

Convenciones de nomenclatura: las constantes de #define o enumeraciones deben
estar todo en mayúscula. Justificación: hace que sea más fácil detectar en tiempo de compilación
constantes en el código fuente.
 e.g. EMC_MESSAGE_TYPE

Las clases y los espacios de nombres deben tener en mayúscula la primera letra de cada palabra y
evitar los guiones bajos. Justificación: identifica clases, constructores y destructores.
 e.g. GtkWidget

Los métodos (o nombres de funciones) deben seguir las recomendaciones C anteriores y
no debe incluir el nombre de la clase. Justificación: mantiene un estilo común de
fuentes C y C ++.
 e.g. get_foo_bar()

Sin embargo, los métodos booleanos son más fáciles de leer si evitan los guiones bajos y usan
el prefijo 'is' (no debe confundirse con los métodos que manipulan un booleano).
Justificación: identifica el valor de retorno como VERDADERO o FALSO y nada más.
 e.g. isOpen, isHomed

NO use "Not" en un nombre booleano, solo genera confusión cuando
se hacen pruebas lógicas
 e.g. isNotOnLimit o is_not_on_limit son incorrectos.

Los nombres de las variables deben evitar el uso de mayúsculas y guiones bajos, excepto
para nombres locales o privados. Se debe evitar el uso de variables globales
lo más posible. Justificación: aclara cuáles son las variables y cuáles
son métodos.
Public:
 e.g. axislimit
Private:
 e.g. maxvelocity_


            Convenciones de nomenclatura de métodos específicos

Los términos get y set deben usarse donde se accede directamente a un atributo.
Justificación: indica el propósito de la función o método.
 e.g. get_foo set_bar

Para los métodos que involucran atributos booleanos, se prefiere set y reset. Razón fundamental:
Como la anterior.
 e.g. set_amp_enable reset_amp_fault

Los métodos intensivos matemáticos deben usar 'compute' como prefijo. Justificación: muestra que
es computacionalmente intensivo y acaparará la CPU.
 e.g. compute_PID

Las abreviaturas en los nombres deben evitarse siempre que sea posible. La excepción es
para nombres de variables locales. Justificación: claridad del código.
 p.ej. se prefiere pointer sobre ptr
      se prefiere compute sobre cmp, compare se prefiere sobre el cmp.

Las enumeraciones y otras constantes pueden tener como prefijo un nombre de tipo común
 e.g. enum COLOR {
	  COLOR_RED, COLOR_BLUE
      };

Se debe evitar el uso excesivo de macros y definiciones: utilizar métodos simples
o funciones es preferible. Justificación: mejora el proceso de depuración.

Include de archivos de encabezado deben estar en la parte superior de un archivo fuente
y no disperso por todo el cuerpo. Deben clasificarse y agruparse
por su posición jerárquica dentro del sistema con los archivos de bajo nivel
incluidos primero. Incluir rutas de archivos NUNCA debe ser absoluto: utilice el
compilador -I flag en su lugar. Justificación: los encabezados pueden no estar en el mismo lugar
en todos los sistemas

Los punteros y las referencias deben tener su símbolo de referencia junto al
nombre de la variable en lugar del nombre del tipo. Justificación: Reduce la confusión.
  e.g. float *x or int &i

Las pruebas implícitas para cero no deben usarse excepto para las variables booleanas.
 e.g. if (spindle_speed != 0) NOT if (spindle_speed)

Solo las declaraciones de control de bucle deben incluirse en una construcción for ().
 e.g. sum = 0;
      for (i = 0; i < 10; i++) {
	  sum += value[i];
      }

NO
      for (i = 0, sum = 0; i < 10; i++) sum += value[i];

Del mismo modo, deben evitarse las declaraciones ejecutables en condicionales.
 e.g. if (fd = open(file_name) is bad.

Deben evitarse las declaraciones condicionales complejas.
variables booleanas temporales en su lugar.

La forma while (true) debe usarse para bucles infinitos.
 e.g. while (true) {
	  ...;
      }

NO

for (;;) {
    ...;
}

o

while (1) {
    ...;
}

Los paréntesis deben usarse en abundancia en las expresiones matemáticas. No confíe
en la precedencia del operador cuando un paréntesis adicional aclararía las cosas

File names: C++ sources and headers use .cc and .hh extension. The use of .c
and .h are reserved for plain C. Headers are for class, method, and structure
declarations, not code (unless the functions are declared inline).
Nombres de archivo: las fuentes y los encabezados de C++ usan la extensión .cc y .hh. El uso de .c
y .h están reservados para el C. Los encabezados son para declaraciones de clase, método y estructura, 
no código (a menos que las funciones se declaren inline).
