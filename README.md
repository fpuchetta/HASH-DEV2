<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TP HASH

## Alumno: Puchetta Federico - 112853 - fpuchetta@fi.uba.ar

- Para compilar junto a main:

```bash
make hash
```

- Para compilar junto a pruebas de usuario:

```bash
make pruebas
```

- Para ejecutar:

```bash
./hash <archivo> <comandos> <modo> <argumento>

comandos: "buscar"
modo: "id" o "nombre"
```

- Para ejecutar pruebas con valgrind:

```bash
make valgrind-alumno
```

- Para limpiar archivos:

```bash
make clean
```

- Para aplicar format:

```bash
make format
```

---

##  Eleccion de estructuras:

Para la implementacion de este TDA, se crearon 2 estructuras principales, y 2 auxiliares.

La estructura `lista_t` fue la elegida como "tabla auxiliar" para cada indice de la tabla de hash y de esta forma poder contrarrestar las colisiones. A pesar de las complejidades de una lista, en este caso no nos afectarian porque justamente la idea de un hash es tratar de estar lo mas cerca a una complejidad computacional O(1).

Por un lado, en las estructuras principales se opto por crear `par_t` y `hash_t` junto con `lista_t` creada en un tp anterior.

La estructura `par_t` Esta pensado para ser la estructura encargada de guardar la informacion de cada par clave-valor insertado en la tabla de hash, cuenta con un string de caracteres que representa la clave y un campo generico para guardar el valor.

La estructura `hash_t` es la encargada de guardar la tabla de hash junto con 2 indicadores de capacidad y cantidad de insertados para tener a mano cuando se debe reashear la tabla.

Por otro lado, en las estructuras auxiliares se opto por crear `h_accionar_aux_t` y `rehash_aux_t`.

La estructura `h_accionar_aux_t` es utilizada para realizar la busqueda insercion y actualizacion de valores en la tabla de hash con una sola iteracion.
Cuenta con un par_t conocido el cual sera el dato que el usuario le pasara a la funcion correspondiente y un slot para guardar el valor anterior o el encontrado en caso de ser necesario.

Finalmente, la estructura `rehash_aux_t` fue pensada para poder realizar la insercion de elementos a la hora de hacer un rehash nuevamente con una sola iteracion, permite recorrer cada lista de la tabla anterior y volver a insertar uno por uno los elementos en la tabla nueva correspondiente.

Una vez dispuesto en memoria, el tda abb_t se veria asi:

<div align="center">
<img src="img/hash_memoria.svg">
</div>

---

## Funcionamiento: (main)

El archivo pedido `main.c` al ser ejecutado primero garantiza que el programa fue invocado con la cantidad de argumentos exactos para la funcionalidad de la busqueda. Luego, lee el archivo y es guardado en la estructura `tp1_t` para luego ser recorrido cada elemento del tp con `tp1_con_cada_pokemon` y llamar a un cb propio encargado de meter cada pokemon en el hash.

Finalmente, se analiza que comando se debe ejecutar y se realiza la busqueda correspondiente.

Para las funcionalidades pedidas se inserta cada par en la tabla con los nombres de los respectivos pokemones como claves y el pokemon en si como valor.

De esta forma, la busqueda mediante nombre posee una complejidad computacional O(1) en casi su totalidad.

Aun asi, la busqueda por id no posee la misma complejidad computacional debido a que los pares fueron insertados mediante el nombre como clave, entonces la funcion de hash no sabria como tratar a las ids de cada pokemon. Debido a esto, se debe buscar en todos los elementos de la tabla tornando la complejidad computacional de la busqueda por id en O(n).

---

## Complejidades de primitivas:

### `hash_t *hash_crear(size_t capacidad_inicial)`:

Esta funcion se encarga de reservar la memoria necesaria para el funcionamiento de la tabla de hash. A pesar de solo contener funciones "constantes", debe inicialiar N cantidad de listas, por lo cual, si tomamos como variable de analisis a la capacidad que proporciona el usuario podriamos decir que **la complejidad computacional de esta funcion es O(n) siendo N la cantidad de listas**.

### `size_t hash_cantidad(hash_t *hash)`:

Esta funcion **posee una complejidad computacional O(1)** debido al campo "insertados" en `hash_t`.

### `bool hash_insertar(hash_t *hash, char *clave, void *valor, void **encontrado)`:

La funcion de insercion en la tabla de hash, consta de una complejidad constante en la gran mayoria de casos gracias a la funcion de hash. En caso de haber colisiones se deben iterar M elementos de la lista acorde al indice de tabla conseguido.

Aun asi, tomando como variable de analisis N:"Cantidad de listas (capacidad)" M resulta insignificante para nuestro analisis.

Sin embargo, la insercion puede complejizarse computacionalmente en caso de que se exceda el **factor de carga maximo** o **la cantidad maxima por lista**. En dicho caso, se debe rehashear, lo cual significaria tener que iterar cada elemento de cada lista y reinsertarlo en una con el doble de capacidad, para lograr amortiguar la cantidad de re-estructuraciones.

De esta forma, **la complejidad computacional de esta funcion resulta en O(n.m) pero M resulta insignificante al lado de N por lo cual es O(n)**.

El siguiente diagrama puede mostrar los pasos que se siguen a la hora de insertar un par clave-valor en la tabla:

<div align="center">
<img src="img/hash_insertar.svg">
</div>

### `void *hash_buscar(hash_t *hash, char *clave)`:

Para buscar un elemento, basta con aplicarle la funcion de hash a la clave proporcionada. De esta forma, caeremos en la lista que posee dicho par. Luego, se itera cada elemento de la lista hasta encontrar el par buscado.

Aun a pesar de tener que recorrer M elementos en dicha lista, resultan insignificantes al lado de las N listas en la tabla. Por lo cual, **se puede decir que la complejidad computacional de esta funcion se acerca a O(1)**, debido a que ya se en que lista debo buscar.

El flujo de una busqueda puede verse asi:
<div align="center">
<img src="img/hash_buscar.svg">
</div>

### `bool hash_contiene(hash_t *hash, char *clave)`:

Esta funcion **posee una complejidad computacional O(1)** debido a que sigue la misma logica que "hash_buscar". La cantidad de elementos por lista es muy chico en comparacion a la cantidad de listas, y esa es la variable de analisis.

### `void *hash_quitar(hash_t *hash, char *clave)`:

A la hora de sacar un elemento de la tabla de hash, **la complejidad computacional de la funcion es O(1)**, debido a que nuevamente se deben iterar M elementos hasta encontrar el indicado y luego se realizan acciones constantes para liberar la memoria relacionada a dicho par.

### `size_t hash_iterar(hash_t *hash, bool (*f)(char *, void *, void *), void *ctx)`:

Esta funcion se encarga de iterar **todos los elementos de la tabla**, por lo cual se debe recorrer en su totalidad. De esta forma **se dice que recorrer los N elementos de la lista tiene una complejidad computacional O(n)**.

### `void hash_destruir(hash_t *hash)` & `void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))`:

Ambas funciones deben recorrer toda la tabla y realizar acciones que resultan constantes frente a nuestra variable de analisis.

De esta forma, al recorrer los N elementos de la tabla **la complejidad computacional de estas funciones resultan en O(n)**.

---

## Respuestas a las preguntas teóricas

- Qué es un diccionario - Explicá 3 formas diferentes de implementar un diccionario (tabla de hash cuenta como 1 sola implementación posible)

Un diccionario es un TDA el cual busca guardar pares asociados de clave-valor. Dependiendo de la forma de implementarlo, se pueden permitir copias de claves o no.

Un diccionario puede implementarse de diversas formas, en este caso hablaremos de **diccionario como lista enlazada**, **diccionario como tabla de hash** y **diccionario como abb balanceado**

#### Diccionario como tabla de hash:

En primer lugar, un diccionario implementado mediante una tabla hash se compone de una tabla destinada a almacenar pares clave-valor y de una funcion de hash encargada de generar un numero dentro del rango de indices disponibles en dicha tabla.

Este metodo de implementacion resulta especialmente util por su alta velocidad de acceso a los datos, alcanzando en la mayoria de los casos una complejidad constante gracias al uso de la funcion de hash. Ademas, ofrece buena escalabilidad, ya que al ampliar el tamaño de la tabla es posible almacenar una mayor cantidad de pares sin modificar el funcionamiento general de la estructura.

Sin embargo, presenta ciertas desventajas frente a las otras alternativas. Al aplicar la funcion de hash pueden producirse colisiones, es decir, casos en los que dos claves distintas generan el mismo valor hash. En tales situaciones, la busqueda deja de ser estrictamente O(1), pues puede requerir una insercion o verificacion adicional en una posicion cercana.

Por otro lado, las tablas deben definirse con una capacidad maxima desde su creacion, lo que puede implicar un uso elevado de memoria si el tamaño elegido es demasiado grande. Ademas, esta estructura no conserva un orden de insercion especifico.

La representacion de esta implementacion puede observarse a continuacion:

<div align="center">
<img src="img/hash_teorica.png">
</div>

---

#### Diccionario como lista enlazada:

Los diccionarios implementados mediante listas enlazadas almacenan los pares clave-valor dentro de una lista, donde cada nodo contiene uno de estos pares.

Entre sus ventajas se destaca la simplicidad de implementacion, ya que no requiere definir una funcion de hash especifica para el problema. Ademas, el uso de memoria resulta eficiente, puesto que solo se asigna espacio para los nodos que efectivamente contienen pares, sin necesidad de reservar un tamaño fijo o maximo.

No obstante, presenta desventajas en cuanto al rendimiento de las operaciones de busqueda, insercion y eliminacion, ya que estas adoptan la complejidad propia de las listas enlazadas, siendo O(n) en la mayoria de los casos.

Adicionalmente, al igual que en una estructura hash, no se conserva un orden determinado de insercion.

La estructura puede representarse de la siguiente manera:

<div align="center">
<img src="img/lista_teorica.png">
</div>

---

#### Diccionario como abb balanceado:

En un diccionario implementado mediante un ABB balanceado, como un AVL o un Arbol Rojo-Negro, se observan ciertas mejoras respecto a la version basada en una lista enlazada, aunque estas no alcanzan el rendimiento de una estructura hash.

Las ventajas frente a la lista enlazada provienen de la complejidad de las operaciones basicas del TDA, ya que heredan las complejidades logaritmicas propias de un ABB balanceado. En cuanto al uso de memoria, resulta similar al de la lista, pues solo se reserva espacio para cada nodo insertado. Ademas, el arbol mantiene un orden definido de insercion, lo que permite realizar busquedas o recorridos en un orden determinado.

Sin embargo, esta implementacion presenta desventajas, como el costo adicional de mantener el balance del arbol durante inserciones y eliminaciones. Aunque el acceso a las claves es logaritmico, no alcanza la eficiencia del acceso constante que ofrece una tabla hash, lo que puede afectar el rendimiento cuando existen muchos pares almacenados.

La estructura puede representarse del siguiente modo:

<div align="center">
<img src="img/abb_teorica.png">
</div>

---

- Qué es una función de hash y qué características debe tener para nuestro problema en particular

Una funcion de hash es un algoritmo que convierte las claves de entrada en valores numericos ubicados dentro del rango de una tabla previamente definida.

El objetivo principal de esta tabla es permitir un acceso rapido a los elementos a traves de sus claves.

Por esta razon, una funcion de hash debe cumplir ciertas propiedades fundamentales: debe ser deterministica, es decir, generar siempre la misma salida para una misma entrada; eficiente, para posibilitar inserciones y busquedas rapidas; y uniforme, de modo que distribuya los valores de salida de manera equitativa y reduzca al minimo las colisiones.

En el caso particular de nuestra implementacion, la funcion de hash debe ser rapida y uniforme, con el fin de lograr una insercion y recuperacion de datos agiles, evitando al mismo tiempo la mayor cantidad posible de colisiones entre pares clave-valor. Esto contribuye ademas a que la operacion de rehash, que suele ser la mas costosa dentro del proceso de insercion, pueda ejecutarse de forma mas eficiente.

---

- Qué es una tabla de Hash y los diferentes métodos de resolución de colisiones vistos.

Una tabla hash es un TDA diseñado para almacenar pares clave-valor, asociandolos a una funcion de hash que determina la posicion de cada clave dentro de una tabla o vector dinamico. El objetivo principal de esta estructura es permitir operaciones de acceso, insercion y eliminacion con una complejidad cercana a O(1), gracias a la distribucion uniforme que proporciona la funcion de hash.

Sin embargo, el uso de un vector dinamico introduce una limitacion: el numero de posiciones disponibles. Cuando la funcion de hash genera un indice que ya esta ocupado por otra clave, se produce una colision.

Para manejar estas colisiones existen dos estrategias principales: direccionamiento abierto y direccionamiento cerrado.

En el direccionamiento abierto, los pares clave-valor no se almacenan necesariamente en la posicion exacta devuelta por la funcion de hash. En su lugar, cuando ocurre una colision, el algoritmo busca una nueva posicion libre siguiendo un esquema de probing (sondeo) predefinido, que puede ser lineal, cuadratico o incluso doble hashing, segun la implementacion. Este metodo permite aprovechar de manera eficiente las posiciones vacias del vector, evitando la necesidad de estructuras adicionales para las colisiones.

No obstante, esta tecnica puede degradar su rendimiento si la tabla se llena demasiado, por lo que resulta esencial mantener un factor de carga adecuado y realizar rehash cuando sea necesario, asegurando una buena distribucion de las claves y evitando bucles de sondeo extensos.

Una tabla hash con direccionamiento abierto (usando probing lineal) puede representarse de la siguiente manera:

<div align="center">
<img src="img/hash_dir_abierto.png">
</div>

Por otro lado, el direccionamiento cerrado es otra estrategia para resolver colisiones, en la cual cada par clave-valor se almacena en el indice exacto devuelto por la funcion de hash. En este enfoque, cada posicion de la tabla (vector) contiene un puntero o referencia hacia una estructura auxiliar —como una lista enlazada, otro hash, o incluso un arbol binario de busqueda (ABB)— donde se agrupan todos los elementos que producen el mismo valor hash.

De esta manera, cuando ocurre una colision, el nuevo par no busca otra posicion en la tabla, sino que se inserta dentro de la estructura asociada al indice correspondiente. Esto permite agrupar eficientemente las colisiones y mantener una organizacion ordenada de los elementos, sin necesidad de recorrer posiciones adicionales del vector principal.

No obstante, el direccionamiento cerrado requiere una gestion cuidadosa de la capacidad de la tabla, ya que si las estructuras asociadas a cada indice comienzan a crecer demasiado, el rendimiento puede degradarse. En esos casos, se recomienda realizar un rehash o redimensionamiento del arreglo principal para redistribuir las claves y conservar un tiempo de acceso cercano a O(1).

Un hash con direccionamiento cerrado (utilizando listas enlazadas) puede representarse de la siguiente manera:

<div align="center">
<img src="img/hash_dir_cerrado.png">
</div>

--- 
- Explicar por qué es importante el tamaño de la tabla (tanto para tablas abiertas como cerradas). Dado que en una tabla abierta se pueden encadenar colisiones sin importar el tamaño de la tabla, ¿Realmente importa el tamaño?

El tamaño de una tabla hash resulta un factor clave tanto en implementaciones con direccionamiento cerrado como con direccionamiento abierto.

En el caso de un hash cerrado, el tamaño de la tabla determina la cantidad de posiciones disponibles para almacenar claves. Una tabla mas grande reduce la probabilidad de colisiones, lo que mejora el rendimiento de las operaciones de insercion y busqueda, manteniendo su complejidad cercana a O(1). En cambio, una tabla demasiado pequeña aumenta las colisiones, lo que puede hacer que la insercion o busqueda de elementos se asemeje a un O(n) debido a la necesidad de recorrer mas posiciones antes de encontrar un lugar vacio.

Por otro lado, en un hash abierto, aunque las colisiones se gestionan mediante estructuras auxiliares (como listas o arboles) en cada indice, el tamaño de la tabla sigue siendo determinante para el desempeño general. Si la tabla permanece fija mientras crece la cantidad de elementos, las estructuras asociadas a cada indice pueden volverse excesivamente grandes, incrementando la complejidad de operaciones como la busqueda o eliminacion.

Por ejemplo, si el direccionamiento cerrado utiliza listas enlazadas para almacenar colisiones, mantener una tabla pequeña provocaria que algunas listas se vuelvan muy largas, degradando la eficiencia hasta O(n), que es la complejidad de recorrer una lista completa.

Por ello, es fundamental ajustar dinamicamente el tamaño de la tabla a medida que aumenta la cantidad de elementos almacenados. Al hacerlo, se conserva una distribucion equilibrada de claves, las listas (o estructuras asociadas) se mantienen cortas, y la complejidad de las operaciones basicas se aproxima nuevamente a O(1), aprovechando mejor el poder de la funcion de hash.


