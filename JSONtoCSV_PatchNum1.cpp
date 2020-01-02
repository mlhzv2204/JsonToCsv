/*
 * Copyright: (2019) Ruslan Malkhozov <vk.com/mlhzv>
 * License: STANKIN-BOMONKA corporated
 *
 * Перекомпановка файла *.json в файл *.csv для работы с календарем
 *
 */

#include <stdio.h> // printf/ scanf
#include <string.h> // работа со строками
#include <errno.h> // Коды ошибок для отладки
#include <stdlib.h> // Для atoi()
#include <windows.h> // Для русского языка в консоле
#include <ctime> // Для clock()
#include <corecrt_io.h> // Для поиска файлов .json в текущей директории папки
#include <chrono>
#include <iostream>

#define CHAR 200
#define CHARTEMP 150 

int main()
{
	auto t1 = std::chrono::high_resolution_clock::now();

	FILE* OutputFile; //Выходной файл, который .csv
	FILE* SourceFile; //Входной файл, который .json

	// ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ ДЛЯ РАБОТЫ СО СТРОКАМИ
	char ReadyString[CHAR]; // Временное хранение строки, в которую записываются данные, состоящая из CHAR символов
	char NameOfJsonFile[CHAR]; // Название исходного *.json файла, который в процессе работы переименовывается в *.csv
	char TempTITLE[CHARTEMP]; // Временно хранится название предмета
	char TempLECTURER[CHARTEMP]; // Временно хранится имя преподавателя
	char TempTYPE[CHARTEMP]; // Временно хранится вид занятия
	char TempSUBGROUP[CHARTEMP]; // Временно хранится информация о проведении занятия для всей группы или подгруппы
	char TempCLASSROOM[CHARTEMP]; // Временно хранится место проведения занятия
	char TempSTART[CHARTEMP]; // Временно хранится время начала занятия
	char TempEND[CHARTEMP]; // Временно хранится время конца занятия
	char TempFREQUENCY[CHARTEMP]; // Временно хранится инфа о проведении занатия каждую неделю, один раз, через каждые две недели
	char TempDATE[CHARTEMP]; // Даты проведения занатий

	// ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ ДЛЯ РАБОТЫ С ДАТАМИ
	char CHARFirstMonth[3]; // Первый строковый месяц
	char CHARFirstDay[3]; // Первый строковый день месяца
	int INTFirstMonth; // Первый номер месяца, с которым производятся расчеты
	int INTFirstDay; // Первый день месяца, с которым производятся расчеты

	char CHARSecondMonth[3]; // Второй строковый месяц
	char CHARSecondDay[3]; // Второй строковый день месяца
	int INTSecondMonth; // Второй номер месяца, с которым сравнивается первый месяц
	int INTSecondDay; // Второй день месяца, с которым сравнивается первый день

	// ОБЪЯВЛЕНИЕ УКАЗАТЕЛЕЙ
	char* p; // указатель для работы с поиском нужного якоря в файле
	char* correct; // указатель необходимый для проставления после найденных необходимых ключевых слов символа конца строки,
				   //после использования strcpy_s, мы тем самым убираем ненужные нам символы после найденного необходимого сочетания символов

	// ОБЪЯВЛЕНИЕ ЯКОРЕЙ ДЛЯ ПОИСКА----------------------------------------------------------
	char TITLE[] = "title"; // Название предмета
	char LECTURER[] = "lecturer"; // ФИО преподавателя
	char TYPE[] = "type"; // Вид занаятия (лекция/семинар/лаба)
	char SUBGROUP[] = "subgroup"; // Занятие для всей группы или для какой-то из подгрупп А или Б
	char CLASSROOM[] = "classroom"; // Номер аудитории, место проведения занятия
	char START[] = "start"; // Время начала занятия
	char END[] = "end"; // Время конца занятия
	char FREQUENCY[] = "frequency"; // Повтор занятия (каждую неделю, один раз, через каждые две недели)
	char DATE[] = "date\""; // Дата/даты проведения занятия/занятий \" нужна для того, чтобы strstr() не путал "date" с "dates"
	//------------------------------------------------------------------ --------------------
	int QuotationMarks = '"'; // Символ кавычки для работы с текстом файла
	errno_t err; // Возвращение ошибок при открытии файлов на ввод вывод

	SetConsoleOutputCP(65001);

	struct _finddata_t data; // Это объявление переменной data, структурного типа _finddata_t (её определение находится в подключаемой библиотеке corecrt_io.h)
	intptr_t hJsonFile; // Это объявление знаковой целочисленной переменной hJsonFile 

	if ((hJsonFile = _findfirst("*.json", &data)) == -1L)
	{
		printf("Не могу найти ни одного файла с расширением *.json в текущей директории\n");
	}
	else
	{
		do
		{
			// Считываю со переменной data структурного типа _finddata_t имя файла в формате *.*
			strcpy_s(NameOfJsonFile, sizeof(NameOfJsonFile), data.name);

			// Открытие "NameOfJsonFile".json файла для считывания данных
			err = fopen_s(&SourceFile, NameOfJsonFile, "r");
			if (err == 0)
			{
				printf("Открыл файл %s\n", NameOfJsonFile);
			}
			else
			{
				printf("Не могу открыть файл %s\n", NameOfJsonFile);
				return 33;
			}

			// Переименовываю *.json файл в *.csv
			NameOfJsonFile[strlen(NameOfJsonFile) - 5] = '\0';
			strcat_s(NameOfJsonFile, sizeof(NameOfJsonFile), ".csv");

			// Открытие "NameOfJsonFile".csv файла для записи данных
			err = fopen_s(&OutputFile, NameOfJsonFile, "w");
			if (err == 0)
			{
				printf("Открыл файл %s\n", NameOfJsonFile);
			}
			else
			{
				printf("Не могу открыть файл %s\n", NameOfJsonFile);
				return 71;
			}

			// Вывод шапки в начало *.csv файла
			fprintf_s(OutputFile, "\"Тема\",\"Дата начала\",\"Время начала\",\"Дата завершения\",\"Время завершения\",\"Целый день\",\"Напоминание вкл / выкл\",\"Дата напоминания\",\"Время напоминания\",\"Организатор собрания\",\"Обязательные участники\",\"Необязательные участники\",\"Ресурсы собрания\",\"В это время\",\"Важность\",\"Категории\",\"Место\",\"Описание\",\"Пометка\",\"Расстояние\",\"Способ оплаты\",\"Частное\"\n");

			// Обработка *.json файла для корректного отображения данных в *.csv
			while (fgets(ReadyString, CHAR, SourceFile) != 0)
			{
				// Поиск слова "title" в строке файла если находит, то сохраняет после себя TempTITLE, в котором хранится название предмета
				if (p = strstr(ReadyString, TITLE))
				{
					p += strlen(TITLE) + 4;
					correct = strchr(p, QuotationMarks);
					*correct = '\0';
					strcpy_s(TempTITLE, sizeof(TempTITLE), p);
				}

				// Поиск слова "lecturer" в строке файла если находит, то сохраняет после себя TempLECTURER, в котором хранится название предмета
				if (p = strstr(ReadyString, LECTURER))
				{
					p += strlen(LECTURER) + 4;
					correct = strchr(p, QuotationMarks);
					*correct = '\0';
					strcpy_s(TempLECTURER, sizeof(TempLECTURER), p);
				}
				
				// Поиск слова "type" в строке файла если находит, то сохраняет после себя TempTYPE, в котором хранится вид занаятия (лекция/семинар/лаба)
				if (p = strstr(ReadyString, TYPE))
				{
					p += strlen(TYPE) + 4;
					correct = strchr(p, QuotationMarks);
					*correct = '\0';
					strcpy_s(TempTYPE, sizeof(TempTYPE), p);
					if (strcmp(TempTYPE, "Lecture") == 0)
						strcpy_s(TempTYPE, sizeof(TempTYPE), "Лекция");
					else if (strcmp(TempTYPE, "Seminar") == 0)
						strcpy_s(TempTYPE, sizeof(TempTYPE), "Семинар");
					else if (strcmp(TempTYPE, "Laboratory") == 0)
						strcpy_s(TempTYPE, sizeof(TempTYPE), "Лаба");
				}

				// Поиск слова "subgroup" в строке файла если находит, то сохраняет после себя TempSUBGROUP, в котором хранится инфа о принадлежности занятия для одной из подгурпп, либо всей группы
				if (p = strstr(ReadyString, SUBGROUP))
				{
					p += strlen(SUBGROUP) + 4;
					correct = strchr(p, QuotationMarks);
					*correct = '\0';
					strcpy_s(TempSUBGROUP, sizeof(TempSUBGROUP), p);
					if (strcmp(TempSUBGROUP, "Common") == 0)
						strcpy_s(TempSUBGROUP, sizeof(TempSUBGROUP), "");
					else if (strcmp(TempSUBGROUP, "A") == 0)
						strcpy_s(TempSUBGROUP, sizeof(TempSUBGROUP), "(А)");
					else if (strcmp(TempSUBGROUP, "B") == 0)
						strcpy_s(TempSUBGROUP, sizeof(TempSUBGROUP), "(Б)");
				}

				// Поиск слова "classroom" в строке файла если находит, то сохраняет после себя TempCLASSROOM, в котором хранится номер аудитории/место проведения занятия
				if (p = strstr(ReadyString, CLASSROOM))
				{
					p += strlen(CLASSROOM) + 4;
					correct = strchr(p, QuotationMarks);
					*correct = '\0';
					strcpy_s(TempCLASSROOM, sizeof(TempCLASSROOM), p);
				}

				// Поиск слова "start" в строке файла если находит, то сохраняет после себя TempSTART, в котором хранится время начала занятия
				if (p = strstr(ReadyString, START))
				{
					p += strlen(START) + 4;
					correct = strchr(p, QuotationMarks);
					*correct = '\0';
					strcpy_s(TempSTART, sizeof(TempSTART), p);
					strcat_s(TempSTART, sizeof(TempSTART), ":00");
				}

				// Поиск слова "end" в строке файла если находит, то сохраняет после себя TempEND, в котором хранится время окончания занятия
				if (p = strstr(ReadyString, END))
				{
					p += strlen(END) + 4;
					correct = strchr(p, QuotationMarks);
					*correct = '\0';
					strcpy_s(TempEND, sizeof(TempEND), p);
					strcat_s(TempEND, sizeof(TempEND), ":00");
				}
				// Поиск слова "frequency" в строке файла если находит, то сохраняет после себя TempFREQUENCY, в котором хранится частота повторения занятия
				if (p = strstr(ReadyString, FREQUENCY))
				{
					p += strlen(FREQUENCY) + 4;
					correct = strchr(p, QuotationMarks);
					*correct = '\0';
					strcpy_s(TempFREQUENCY, sizeof(TempFREQUENCY), p);
				}

				// Поиск слова "date" в строке файла если находит, то сохраняет после себя TempDATE, в котором хранится дата/даты проведения занятия или занятий
				if (p = strstr(ReadyString, DATE))
				{
					p += strlen(DATE) + 3;
					correct = strchr(p, QuotationMarks);
					*correct = '\0';
					strcpy_s(TempDATE, sizeof(TempDATE), p);
					// В зависимости от TempFREQUENCY выводится корректная дата в файл
					if (strcmp(TempFREQUENCY, "once") == 0)
					{
						if (strlen(TempCLASSROOM) == 0)
							fprintf_s(OutputFile, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", TempTITLE, TempDATE, TempSTART, TempDATE, TempEND, TempTYPE, TempLECTURER);
						else
							fprintf_s(OutputFile, "\"%s. %s%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", TempTITLE, TempCLASSROOM, TempSUBGROUP, TempDATE, TempSTART, TempDATE, TempEND, TempTYPE, TempLECTURER);
					}
					else if (strcmp(TempFREQUENCY, "every") == 0 || strcmp(TempFREQUENCY, "throughout") == 0)
					{
						p += 5;
						strncpy_s(CHARFirstMonth, 3, p, 2);
						p += 3;
						strncpy_s(CHARFirstDay, 3, p, 2);
						p += 8;
						strncpy_s(CHARSecondMonth, 3, p, 2);
						p += 3;
						strncpy_s(CHARSecondDay, 3, p, 2);

						INTFirstMonth = atoi(CHARFirstMonth);
						INTFirstDay = atoi(CHARFirstDay);
						INTSecondMonth = atoi(CHARSecondMonth);
						INTSecondDay = atoi(CHARSecondDay);
						do
						{
							if (strlen(TempCLASSROOM) == 0)
								fprintf_s(OutputFile, "\"%s%s\",\"2019.%02d.%02d\",\"%s\",\"2019.%02d.%02d\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", TempTITLE, TempSUBGROUP, INTFirstMonth, INTFirstDay, TempSTART, INTFirstMonth, INTFirstDay, TempEND, TempTYPE, TempLECTURER);
							else
								fprintf_s(OutputFile, "\"%s. %s%s\",\"2019.%02d.%02d\",\"%s\",\"2019.%02d.%02d\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", TempTITLE, TempCLASSROOM, TempSUBGROUP, INTFirstMonth, INTFirstDay, TempSTART, INTFirstMonth, INTFirstDay, TempEND, TempTYPE, TempLECTURER);
							if (strcmp(TempFREQUENCY, "every") == 0)
								INTFirstDay += 7;
							else if (strcmp(TempFREQUENCY, "throughout") == 0)
								INTFirstDay += 14;
							if (INTFirstMonth == 1 || INTFirstMonth == 3 || INTFirstMonth == 5 || INTFirstMonth == 7 || INTFirstMonth == 8 || INTFirstMonth == 10 || INTFirstMonth == 12)
							{
								if (INTFirstDay > 31)
								{
									INTFirstDay -= 31;
									++INTFirstMonth;
								}
							}
							else if (INTFirstMonth == 4 || INTFirstMonth == 6 || INTFirstMonth == 9 || INTFirstMonth == 11)
							{
								if (INTFirstDay > 30)
								{
									INTFirstDay -= 30;
									++INTFirstMonth;
								}
							}
							else if (INTFirstMonth == 2)
							{
								if (INTFirstDay > 29)
								{
									INTFirstDay -= 29;
									++INTFirstMonth;
								}
							}
						} while (INTFirstMonth != INTSecondMonth || INTFirstDay != INTSecondDay);
						if (strlen(TempCLASSROOM) == 0)
							fprintf_s(OutputFile, "\"%s%s\",\"2019.%02d.%02d\",\"%s\",\"2019.%02d.%02d\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", TempTITLE, TempSUBGROUP, INTFirstMonth, INTFirstDay, TempSTART, INTFirstMonth, INTFirstDay, TempEND, TempTYPE, TempLECTURER);
						else
							fprintf_s(OutputFile, "\"%s. %s%s\",\"2019.%02d.%02d\",\"%s\",\"2019.%02d.%02d\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", TempTITLE, TempCLASSROOM, TempSUBGROUP, INTFirstMonth, INTFirstDay, TempSTART, INTFirstMonth, INTFirstDay, TempEND, TempTYPE, TempLECTURER);
					}
				}
			}
			fclose(SourceFile); // Закрытие потока с исходным файлом .json
			fclose(OutputFile); // Закрытие потока с выходным файлом .csv
		} while (_findnext(hJsonFile, &data) == 0);
	}
	_findclose(hJsonFile); // Закрытие знаковой целочисленной переменной hJsonFile 
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> fp_ms = t2 - t1;
	std::cout << "Time : " << fp_ms.count() << " msec" << std::endl;
	system("pause");
	return 0;
}