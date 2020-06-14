Estas carpetas contienen la biblioteca requerida (en forma binaria) para que designer use
widgets Python2.

Necesarios para mostrar widgets pyqt:
python-pyqt5
python-pyqt5.qsci
python.pyqt5.qtopengl
python-pyqt5.qtsvg
python-opengl
python-opencv

Opcional:
espeak
espeak-ng-espeak
python-dbus.mainloop.pyqt

debe tener designer instalado:
sudo apt-get install qttools5-dev-tools
sudo apt-get install qttools5.dev

debe copiar esa versión adecuada de libpyqt5_py2.so en la carpeta:
/usr/lib/x86_64-linux-gnu/qt5/plugins/designer
(x86_64-linux-gnu podría llamarse algo ligeramente diferente
en diferentes sistemas)

Necesitará privilegios de superusuario para copiar el archivo en la carpeta.

Este archivo puede incluirse en linuxcnc en esta ubicación:

en una versión RIP:
lib/python/qtvcp/designer/x86_64
o en una versión instalada:
/usr/lib/python2.7/dist-packages/qtvcp/designer/x86_64/

Debe elegir la serie 5.5 o 5.7 o 5.9 de Qt
actualmente Debian Stretch usa 5.7, Mint 12 usa 5.5
en caso de duda verifique la versión qt5 en el sistema
Si necesita compilar la biblioteca para una versión determinada, consulte:
https://gist.github.com/KurtJacobson/34a2e45ea2227ba58702fc1cb0372c40

entonces debe vincular qtvcp_plugin.py a la carpeta que buscará designer.
(El nombre del enlace debe tener .py como final).
Qtvcp_plugin se puede encontrar en:
en versión RIP:
lib/python/qtvcp/plugins
en una versión instalada:
/usr/lib/python2.7/dist-packages/qtvcp/plugins

El enlace se puede colocar en:
/usr/lib/x86_64-linux-gnu/qt5/plugins/designer/python
o
~/.designer/plugins/python

abra una terminal, configure el entorno para linuxcnc (.scripts/rip-environment si RIP)
luego cargue el diseñador con: designer -qt = 5
