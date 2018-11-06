# protos-tpe
## Materiales

* Archivo de construcción: `CMakeLists.txt`, ubicado en el directorio raíz.
* Informe: `BLANKBLANLBLANK`.
* Códigos fuente: carpetas `Control`, `Proxy`, `Parser` y `MediaTypes`.

## Compilación

Para compilar correr en el directorio raíz:

```
./build
```

### Artefactos generados

Se generan cuatro binarios en la raíz del directorio con los nombres:

* pop3filter: server proxy.
* pop3ctl: cliente de configuración.
* parser: programa que ejecuta un comando compatible con system(3)
* stripmime: filtro de media types.

## Ejecución
### pop3filter
El proxy pop3 se ejecuta respetando las opciones y el argumento que sugiere el
manual `pop3filter.8`.
```
./pop3filter [options] <origin-server>
```
### stripmime
Utiliza las variables de entorno definidas por el manual `pop3filter.8`.
Se ejecuta corriendo:
```
./stripmime
```
### pop3ctl
El cliente de configuración se ejecuta corriendo:
```
./pop3ctl
```
Las opciones disponibles son:

* -l \<management_address\> : dirección del server de management
* -p \<management_port\> : puerto del server de management

El usuario y la contraseña para configuración se encuentran en `secret.txt`.