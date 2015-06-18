Wizard BASIC 3 &mdash; компилируемый высокоуровневый язык программирования.
Поддерживает структурную парадигму, обладает строгой динамической типизацией и
сборкой мусора.

### Исходники

Файлы исходного кода должны быть в кодировке ASCII.

### Ключевые слова

17 ключевых слов: `NULL`, `and`, `break`, `continue`, `do`, `else`, `end`,
`function`, `if`, `let`, `new`, `not`, `or`, `return`, `structure`, `then`,
`while`.

### Типы

Типизация: строгая динамическая.

Менеджмент памяти: сборка мусора.

#### Нулевой тип

Название: null.

Тип: фиктивный тип; не хранит значение, а служит маркером его отсутствия.

Копирование: по значению.

Хранение: на стеке.

Определение: `"NULL"`.

#### Числа

Название: number.

Тип: с плавающей запятой, 8 байт.

Копирование: по значению.

Хранение: на стеке.

Определение:

    `/'(\\.|[^'])'/`
    `/\d+(\.\d+)?/`

#### Массивы

Название: array.

Тип: динамический массив.

Копирование: по ссылке.

Хранение:

* ссылки &mdash; на стеке;
* значения &mdash; в куче.

Определение:

    "new", "array", "(", expression, ")"
    "[", [expression, {",", expression}], "]"

Доступ к элементам: `expression, "[", expression, "]"`.

#### Строки

Строками являются массивы, хранящие коды символов. Используется кодировка ASCII.

Определение: `/"(\\.|[^"])*"/`.

#### Структуры

Название: задаётся программистом.

Копирование: по ссылке.

Хранение:

* ссылки &mdash; на стеке;
* значения &mdash; в куче.

Объявление:

    "structure", identifier,
        identifier,
        {identifier},
    "end"

Может располагаться только в глобальной области видимости.

Определение: `"new", identifier`.

Доступ к полям: `expression, ".", identifier`.

#### Логические значения

В качестве ложного логического значения принимаются число 0 и значение `NULL`.

В качестве истинного логического значения принимаются числа, отличные от 0, и
любые ссылки на массивы и структуры.

### Операции

Перечислены в порядке убывания приоритета.

Приоритет | Операция | Описание | Ассоциативность | Типы | Особенности
--- | --- | --- | --- | --- | ---
1 | `[]` | доступ к элементам массива | левая | массивы | &mdash;
1 | `.` | доступ к полям структур | левая | структуры | &mdash;
2 | `new` | определение структур | &mdash; | идентификаторы структур | &mdash;
2 | `-` | унарный минус | правая | числа | &mdash;
2 | `not` | логическое отрицание | правая | все | &mdash;
3 | `*` | умножение | левая | числа | &mdash;
3 | `/` | деление | левая | числа | &mdash;
3 | `%` | остаток от деления | левая | числа | &mdash;
4 | `+` | сложение | левая | числа | &mdash;
4 | `-` | вычитание | левая | числа | &mdash;
5 | `<` | меньше | левая | числа | &mdash;
5 | `<=` | меньше или равно | левая | числа | &mdash;
5 | `>` | больше | левая | числа | &mdash;
5 | `>=` | больше или равно | левая | числа | &mdash;
6 | `==` | равенство | левая | все | при сравнении массивов и структур сравнивает ссылки
6 | `/=` | неравенство | левая | все | при сравнении массивов и структур сравнивает ссылки
7 | `and` | логическое и | левая | все | &mdash;
8 | `or` | логическое или | левая | все | &mdash;

### Переменные

Объявление и определение: `"let", identifier, "=", expression`.

### Присваивание

    expression, "=", expression

В качестве левого `expression` могут использоваться переменные, элементы
массивов и поля структур.

### Управляющие конструкции

#### Условие

    "if", expression, "then",
        statement,
        {statement},
    {"else", "if", expression, "then",
        statement,
        {statement}},
    ["else",
        statement,
        {statement}],
    "end"

#### Цикл

    "while", expression, "do",
        statement,
        {statement},
    "end"

Переход к следующей итерации цикла: `continue`.

Прерывание цикла: `break`.

### Функции

Объявление:

    "function", identifier, "(", [identifier, {",", identifier}], ")",
        statement,
        {statement},
    "end"

Может располагаться только в глобальной области видимости.

Перегрузка функций не поддерживается.

Возврат из функции: `"return", [expression]`. По умолчанию возвращается `NULL`.

Вызов функции: `identifier, "(", [expression, {",", expression}], ")"`. При
вызове функции проверяется соответствие числа аргументов числу параметров.

### Точка входа

    function Main(arguments)

В `arguments` передаются аргументы командной строки. В `arguments[0]` передаётся
путь к исполняемому файлу.

### Комментарии

Однострочные: `? /^\s*note\b[^\n]*\n/ ?`.

Многострочные: `? /^\s*long\s+note\b.*?\.\.\.\s*$/ ?`.

### Модульность

Тип: включением кода как текста.

Использование: `"include", expression`.

В качестве `expression` может использоваться лишь определение строки. Строка
должна содержать относительный путь к файлу с кодом.

Файлы включаются рекурсивно, и каждый только один раз.

### Рантайм

#### Модуль `lang`

* `ToString(number, precision)`;
* `GetSize(array)`;
* `GetType(value)` &mdash; возвращает имя типа переданного значения.

#### Модуль `system`

* `GetTime()`;
* `Exit(exit_code)`.

##### Модуль `system.io`

* `Read(stream_id, number_of_bytes)`;
* `Write(stream_id, bytes)`;
* `Open(path, mode)` &mdash; режим открытия может быть:
    * 0 (чтение);
    * 1 (запись);
    * 2 (дополнение);
* `Close(stream_id)`.

По умолчанию доступны три уже открытых потока:

* 0 &mdash; stdin;
* 1 &mdash; stdout;
* 2 &mdash; stderr.

#### Модуль `maths`

* `Sin(number)`;
* `Cos(number)`;
* `Tg(number)`;
* `Arcsin(number)`;
* `Arccos(number)`;
* `Arctg(number)`;
* `SquareRoot(number)`;
* `Power(base, exponent)`;
* `Exp(number)`;
* `Ln(number)`;
* `Integral(number)`;
* `GetRandom()` &mdash; возвращает случайное число в диапазоне [0; 1].

### Стандартная библиотека

Если в описании функции аргументы взяты в квадратные скобки, это означает, что
функция ожидает не список аргументов, а массив с аргументами.

#### Модуль `general`

##### Константы

* `FALSE`;
* `TRUE`;
* `EXIT_CODE_SUCCESS`;
* `EXIT_CODE_FAILURE`.

#### Модуль `array`

##### Модуль `array/number`

###### Константы

* `COMPARE_RESULT_LESS`;
* `COMPARE_RESULT_EQUAL`;
* `COMPARE_RESULT_GREATER`.

###### Функции

* `Compare(array_1, array_2)`;
* `IsEqual(array_1, array_2)`;
* `ArrayNumberToString([array, precision = DEFAULT_NUMBER_PRECISION])`.

##### Модуль `array/string`

###### Функции

* `Join(array, separator)`;
* `Escape(string)`;
* `ArrayStringToString(array)`.

##### Модуль `array/utils`

###### Функции

* `Copy(array)`;
* `DeepCopy(array)`;
* `Append(array_1, array_2)`;
* `MultiAppend(arrays)`;
* `Slice([array, start = 0, finish = GetSize(array)])`.

#### Модуль `range`

##### Модуль `range/utils`

###### Интерфейсы

* `Range` (см. ниже).

###### Функции

* `Range([minimum, maximum, step = 1, reverse = FALSE])`;
* `Range([array, start = 0, finish = GetSize(array), reverse = FALSE])`;
* `Next(range)`.

#### Модуль `args`

##### Функции

* `Arg([array, index, default_value = NULL])` &mdash; возвращает элемент массива
по индексу; если элемента нет, возвращает значение по умолчанию.

#### Модуль `io`

##### Константы

* `STANDART_STREAM_INPUT`;
* `STANDART_STREAM_OUTPUT`;
* `STANDART_STREAM_ERROR`;
* `FILE_OPEN_MODE_READ`;
* `FILE_OPEN_MODE_WRITE`;
* `FILE_OPEN_MODE_APPEND`.

##### Функции

* `Show(message)`;
* `ShowLn(message)`;
* `Die(message)` &mdash; пишет сообщение в stderr и завершает программу с кодом
`EXIT_CODE_FAILURE`;
* `Error(message)` &mdash; форматирует сообщение об ошибке стандартным для языка
образом, затем вызывает `Die()`;
* `Input(number_of_bytes)`;
* `InputLn()`.

#### Модуль `maths`

##### Константы

* `DEFAULT_NUMBER_PRECISION = 2`.

##### Функции

* `Module(number)` &mdash; возвращает модуль числа;
* `Clamp([number, minimum = NULL, maximum = NULL])`;
* `ParseNumber(text)` &mdash; парсит число в формате `/-?\d*\.?\d*/`;
останавливает парсинг при первом неподходящем символе; при полном отсутствии
числа в начале текста возвращает 0.

#### Модуль `null`

##### Функции

* `IfNull(value, default_value)` &mdash; возвращает значение, если оно не равно
`NULL`, в противном случае возвращает значение по умолчанию.

#### Модуль `ref`

##### Структуры

* `Reference` (см. ниже).

##### Функции

* `Ref(value)` &mdash; обрачивает значение в структуру `Reference`;
* `Deref(reference)` &mdash; если значение обёрнуто в структуру `Reference`,
разворачивает его; в противном случае возвращает без изменений.

#### Модуль `type`

##### Функции

* `HasType(value, type_name)`.

#### Дополнительно

Интерфейс `Range`:

    structure Range
        size  note количество элементов в Range
        value note значение текущего элемента Range
        first note TRUE, если текущий элемент - первый в Range
        last  note TRUE, если текущий элемент - последний в Range
        index note индекс текущего элемента Range (начинается с 0)
    end

Структура `Reference`:

    structure Reference
        value
    end
