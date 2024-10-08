### Ошибки
1.1) is_pow_of_2 - ошибка, есть контрпример. \
1.2) Ломается при подаче на вход int'a. 

2.1) Попробуйте вызвать get(10,8). У меня сломалось. \
2.2) Сомнительный результат:   
```
  uint64_t init_arr[] = { 0x1122334455667788, 0xbabadedacafebeef, 0x4354659710928523, 0xcafebabe8794abcd };
  bits my_bits(256, init_arr);
  my_bits.set(10, my_bits.get(0,8));
  std::cout << "my_bits = " << my_bits.str() << std::endl;
```

```
> my_bits = 8862265544332211EFBEFECADADEBABA2385921097655443CDAB9487BEBAFECA
```

3 ) Принято.


### Отчет

1) Полагаю ошибка была в том, что не учитывалась особеность хранения отрицательных чисел, и поэтому число `int a = (1<<31); // == -1` считалось как точная степень двойки. \
Все изменения - это добавление следующих строчек в начало ф-ии:
```
  T zero = 0;
  if( a < zero ) return 0;
```

2) Изначально я считывал массив переменных интегрального типа как сырые данные - смотрел как на char*. \
По примеру 2.2) можно понять, что байты расположенны в порядке увелечения старшинства (т.е. сначала идет младший байт, потом ..., и в конце старший байт) - в итоге значение для `0x1122334455667788` "разворачивается" в `0x8877665544332211`. \
Чтобы исправить это, и не привязываться к определеному типу хранения, реализовал функцию `reap()` - которая считывает биты, с помощью битовой-маски и смещений. \
Так же оставил возможность "сырого" считывания байтов, передавая в конструкторе флаг `raw_copy = 1`. \
Теперь работает ожидаемо, и get(10,8) вроде работает (в тесты test_get_all_diapasons и test_set_all_diapasons вполне верится).
```c++
  int x = 0x12345678;
  bits a(19, &x);
  // .str(1) - на каждый символ по 1 биту - т.е. вывод в двоичной системе
  cout << "a is: " << a.str(1) << endl; 
  cout << "get : " << a.get(10, 8).str(1) << endl;
```
```
> a is: 0001001000110100010
> get : 11010001
```
( Там правда в set и get нет никакой проверки на выход за границы. )


### Задание

* Реализовать функцию от аргумента $a$ интегрального типа, возвращающую
признак того, что $a$ является степенью двойки: 
$$f (a) == (\exists i \in \mathbb{N} \cup {0} : a == 2^i).$$

$\text{ }$

* Реализовать класс битового вектора произвольной разрядности. \
Необходимые методы:
   - конструктор (разрядность, массив переменных интегрального типа);
   - разрядность ();
   - установить битовое поле (смещение, битвектор);
   - получить битовое поле (смещение, разрядность);
   - получить битовое поле (смещение, разрядность) в переменную
интегрального типа: отдельный вариант для битовых полей малой
разрядности (до 64 разрядов);
   - получить строковое представление в 16-ричном виде.
  Разрядное (битовое) поле битового вектора определяется номерами
позиций младшего и старшего разряда поля.

$\text{ }$

* Реализовать функцию, для данного набора входных элементов $\mathcal{A}$ 
возвращающую случайную выборку из $n$ элементов ( $n \le \text{size of} (\mathcal{A})$ ). \
Все возможные выборки должны быть равновероятны. 
Элементы входного набора считать попарно различными.