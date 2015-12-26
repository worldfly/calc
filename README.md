[![Build Status](https://ci.worldfly.org/buildStatus/icon?job=calc)](https://ci.worldfly.org/job/calc)

Приложение принимает от пользователя строку с арифметическим выражением, содержащим простейшие арифметические операции и скобки, а в ответ выводит результат вычислений.

Предусмотреть возможность ввода в выражение ограниченного списка функций (например, тригонометрических, возведения в степень, логарифмов и т.п.)

Предусмотреть возможность ввода в выражение предварительно определённых пользователем переменных, например:

```
> let a = 5;
= 5
> a;
= 5
> a*2;
= 10
> let b = 2;
= 2
> a*b;
= 10
> let c = 34;
= 34
> cos c;
= -0.84857
> (cos c)^2 + (sin c)^2;
= 1
> q
```