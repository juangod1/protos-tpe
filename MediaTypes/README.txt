# MediaTypes
Un programa que recibe por argumento de línea de comando una lista de media-ranges y por entrada estándar un  media-type por cada línea. El programa imprimirá en la salida estándar por cada media-type recibido los literales  true ,  false ,  null  según esté incluido por los  media-range  o la entrada esté mal formada.

Para compilar:
$> gcc -c *.c
$> gcc -o prog *.o

Ejemplo de ejecucion:

$>./prog 'text/*, application/*'
hola mundo
text/html
audio/plain
application
^D
null
true
false
true
$>
