/*
 * Перекомпановка файла *.json в файл *.csv (Google Calendar)
 */

#include <stdio.h> // printf/ scanf
#include <string.h> // работа со строками
#include <errno.h> // Коды ошибок для отладки
#include <stdlib.h> // Для atoi()
#include <windows.h> // Для русского языка в консоле
#include <corecrt_io.h> // Для поиска файлов .json в текущей директории папки
#include "getCPUTime.c"

#define CHARTEMP 350 // буфер
#define FEBRUARY 29 // для високосных 29, для остальных 28
#define YEAR "2020" // выбор года

int main()
{
	SetConsoleOutputCP(65001);
	SetConsoleCP(65001);

	double startTime, endTime;
	startTime = getCPUTime();
	FILE* OutputFile; //Выходной файл, который .csv
	FILE* SourceFile; //Входной файл, который .json

	// ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ ДЛЯ РАБОТЫ СО СТРОКАМИ
	char ready_string[CHARTEMP]; // Временное хранение строки, в которую записываются данные, состоящая из CHARTEMP символов
	char name_of_json_file[CHARTEMP]; // Название исходного *.json файла, который в процессе работы переименовывается в *.csv
	char temp_title[CHARTEMP]; // Временно хранится название предмета
	char temp_lecturer[CHARTEMP]; // Временно хранится имя преподавателя
	char temp_type[CHARTEMP]; // Временно хранится вид занятия
	char temp_subgroup[CHARTEMP]; // Временно хранится информация о проведении занятия для всей группы или подгруппы
	char temp_classroom[CHARTEMP]; // Временно хранится место проведения занятия
	char temp_start[CHARTEMP]; // Временно хранится время начала занятия
	char temp_end[CHARTEMP]; // Временно хранится время конца занятия
	char temp_frequency[CHARTEMP]; // Временно хранится инфа о проведении занатия каждую неделю, один раз, через каждые две недели
	char temp_date[CHARTEMP]; // Даты проведения занатий

	// ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ ДЛЯ РАБОТЫ С ДАТАМИ
	char char_first_month[3]; // Первый строковый месяц
	char char_first_day[3]; // Первый строковый день месяца
	int int_first_month; // Первый номер месяца, с которым производятся расчеты
	int int_first_day; // Первый день месяца, с которым производятся расчеты

	char char_second_month[3]; // Второй строковый месяц
	char char_second_day[3]; // Второй строковый день месяца
	int int_second_month; // Второй номер месяца, с которым сравнивается первый месяц
	int int_second_day; // Второй день месяца, с которым сравнивается первый день

	// ОБЪЯВЛЕНИЕ УКАЗАТЕЛЕЙ
	char* p; // указатель для работы с поиском нужного якоря в файле
	char* correct; // указатель необходимый для проставления после найденных необходимых ключевых слов символа конца строки,
				   //после использования strcpy_s, мы тем самым убираем ненужные нам символы после найденного необходимого сочетания символов

	// ОБЪЯВЛЕНИЕ ЯКОРЕЙ ДЛЯ ПОИСКА----------------------------------------------------------
	char title[] = "title"; // Название предмета
	char lecturer[] = "lecturer"; // ФИО преподавателя
	char type[] = "type"; // Вид занаятия (лекция/семинар/лаба)
	char subgroup[] = "subgroup"; // Занятие для всей группы или для какой-то из подгрупп А или Б
	char classroom[] = "classroom"; // Номер аудитории, место проведения занятия
	char start[] = "start"; // Время начала занятия
	char end[] = "end"; // Время конца занятия
	char frequency[] = "frequency"; // Повтор занятия (каждую неделю, один раз, через каждые две недели)
	char date[] = "date\""; // Дата/даты проведения занятия/занятий \" нужна для того, чтобы strstr() не путал "date" с "dates"
	//------------------------------------------------------------------ --------------------
	int double_marks = '"'; // Символ кавычки для работы с текстом файла
	errno_t err; // Возвращение ошибок при открытии файлов на ввод вывод

	int number_of_saved_files = 0; // для счетчика кол-ва обработанных файлов

	struct _finddata_t data; // Это объявление переменной data, структурного типа _finddata_t (её определение находится в подключаемой библиотеке corecrt_io.h)
	intptr_t hJsonFile; // Это объявление знаковой целочисленной переменной hJsonFile 

	if ((hJsonFile = _findfirst("*.json", &data)) == -1L)
	{
		printf("Не найдено ни одного файла с расширением *.json в текущей директории\n");
	}
	else
	{
		do
		{
			// Считываю с переменной data структурного типа _finddata_t имя файла в формате *.*
			strcpy_s(name_of_json_file, sizeof(name_of_json_file), data.name);

			// Открытие "name_of_json_file".json файла для считывания данных
			err = fopen_s(&SourceFile, name_of_json_file, "r");
			if (err != 0)
			{
				printf("Не могу открыть файл %s\n", name_of_json_file);
				return 2;
			}

			// Переименовываю *.json файл в *.csv
			name_of_json_file[strlen(name_of_json_file) - 5] = '\0';
			strcat_s(name_of_json_file, sizeof(name_of_json_file), ".csv");

			// Открытие "name_of_json_file".csv файла для записи данных
			err = fopen_s(&OutputFile, name_of_json_file, "w");
			if (err != 0)
			{
				printf("Не могу открыть файл %s\n", name_of_json_file);
				return 3;
			}

			// Вывод шапки в начало *.csv файла
			fprintf_s(OutputFile, "\"Тема\",\"Дата начала\",\"Время начала\",\"Дата завершения\",\"Время завершения\",\"Целый день\",\"Напоминание вкл / выкл\",\"Дата напоминания\",\"Время напоминания\",\"Организатор собрания\",\"Обязательные участники\",\"Необязательные участники\",\"Ресурсы собрания\",\"В это время\",\"Важность\",\"Категории\",\"Место\",\"Описание\",\"Пометка\",\"Расстояние\",\"Способ оплаты\",\"Частное\"\n");

			// Обработка *.json файла для корректного отображения данных в *.csv
			while (fgets(ready_string, CHARTEMP, SourceFile) != 0)
			{
				// Поиск слова "title" в строке файла если находит, то сохраняет после себя temp_title, в котором хранится название предмета
				if (p = strstr(ready_string, title))
				{
					p += strlen(title) + 4;
					correct = strchr(p, double_marks);
					*correct = '\0';
					strcpy_s(temp_title, sizeof(temp_title), p);
				}

				// Поиск слова "lecturer" в строке файла если находит, то сохраняет после себя temp_lecturer, в котором хранится название предмета
				if (p = strstr(ready_string, lecturer))
				{
					p += strlen(lecturer) + 4;
					correct = strchr(p, double_marks);
					*correct = '\0';
					strcpy_s(temp_lecturer, sizeof(temp_lecturer), p);
				}
				
				// Поиск слова "type" в строке файла если находит, то сохраняет после себя temp_type, в котором хранится вид занаятия (лекция/семинар/лаба)
				if (p = strstr(ready_string, type))
				{
					p += strlen(type) + 4;
					correct = strchr(p, double_marks);
					*correct = '\0';
					strcpy_s(temp_type, sizeof(temp_type), p);
					if (strcmp(temp_type, "Lecture") == 0)
						strcpy_s(temp_type, sizeof(temp_type), "Лекция");
					else if (strcmp(temp_type, "Seminar") == 0)
						strcpy_s(temp_type, sizeof(temp_type), "Семинар");
					else if (strcmp(temp_type, "Laboratory") == 0)
						strcpy_s(temp_type, sizeof(temp_type), "Лаба");
				}

				// Поиск слова "subgroup" в строке файла если находит, то сохраняет после себя temp_subgroup, в котором хранится инфа о принадлежности занятия для одной из подгурпп, либо всей группы
				if (p = strstr(ready_string, subgroup))
				{
					p += strlen(subgroup) + 4;
					correct = strchr(p, double_marks);
					*correct = '\0';
					strcpy_s(temp_subgroup, sizeof(temp_subgroup), p);
					if (strcmp(temp_subgroup, "Common") == 0)
						strcpy_s(temp_subgroup, sizeof(temp_subgroup), "");
					else if (strcmp(temp_subgroup, "A") == 0)
						strcpy_s(temp_subgroup, sizeof(temp_subgroup), "(А)");
					else if (strcmp(temp_subgroup, "B") == 0)
						strcpy_s(temp_subgroup, sizeof(temp_subgroup), "(Б)");
				}

				// Поиск слова "classroom" в строке файла если находит, то сохраняет после себя temp_classroom, в котором хранится номер аудитории/место проведения занятия
				if (p = strstr(ready_string, classroom))
				{
					p += strlen(classroom) + 4;
					correct = strchr(p, double_marks);
					*correct = '\0';
					strcpy_s(temp_classroom, sizeof(temp_classroom), p);
				}

				// Поиск слова "start" в строке файла если находит, то сохраняет после себя temp_start, в котором хранится время начала занятия
				if (p = strstr(ready_string, start))
				{
					p += strlen(start) + 4;
					correct = strchr(p, double_marks);
					*correct = '\0';
					strcpy_s(temp_start, sizeof(temp_start), p);
					strcat_s(temp_start, sizeof(temp_start), ":00");
				}

				// Поиск слова "end" в строке файла если находит, то сохраняет после себя temp_end, в котором хранится время окончания занятия
				if (p = strstr(ready_string, end))
				{
					p += strlen(end) + 4;
					correct = strchr(p, double_marks);
					*correct = '\0';
					strcpy_s(temp_end, sizeof(temp_end), p);
					strcat_s(temp_end, sizeof(temp_end), ":00");
				}
				// Поиск слова "frequency" в строке файла если находит, то сохраняет после себя temp_frequency, в котором хранится частота повторения занятия
				if (p = strstr(ready_string, frequency))
				{
					p += strlen(frequency) + 4;
					correct = strchr(p, double_marks);
					*correct = '\0';
					strcpy_s(temp_frequency, sizeof(temp_frequency), p);
				}

				// Поиск слова "date" в строке файла если находит, то сохраняет после себя temp_date, в котором хранится дата/даты проведения занятия или занятий
				if (p = strstr(ready_string, date))
				{
					p += strlen(date) + 3;
					correct = strchr(p, double_marks);
					*correct = '\0';
					strcpy_s(temp_date, sizeof(temp_date), p);
					// В зависимости от temp_frequency выводится корректная дата в файл
					if (strcmp(temp_frequency, "once") == 0)
					{
						if (strlen(temp_classroom) == 0)
							fprintf_s(OutputFile, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", temp_title, temp_date, temp_start, temp_date, temp_end, temp_type, temp_lecturer);
						else
							fprintf_s(OutputFile, "\"%s. %s%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", temp_title, temp_classroom, temp_subgroup, temp_date, temp_start, temp_date, temp_end, temp_type, temp_lecturer);
					}
					else if (strcmp(temp_frequency, "every") == 0 || strcmp(temp_frequency, "throughout") == 0)
					{
						p += 5;
						strncpy_s(char_first_month, 3, p, 2);
						p += 3;
						strncpy_s(char_first_day, 3, p, 2);
						p += 8;
						strncpy_s(char_second_month, 3, p, 2);
						p += 3;
						strncpy_s(char_second_day, 3, p, 2);

						int_first_month = atoi(char_first_month);
						int_first_day = atoi(char_first_day);
						int_second_month = atoi(char_second_month);
						int_second_day = atoi(char_second_day);
						do
						{
							if (strlen(temp_classroom) == 0)
								fprintf_s(OutputFile, "\"%s%s\",\""YEAR".%02d.%02d\",\"%s\",\""YEAR".%02d.%02d\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", temp_title, temp_subgroup, int_first_month, int_first_day, temp_start, int_first_month, int_first_day, temp_end, temp_type, temp_lecturer);
							else
								fprintf_s(OutputFile, "\"%s. %s%s\",\""YEAR".%02d.%02d\",\"%s\",\""YEAR".%02d.%02d\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", temp_title, temp_classroom, temp_subgroup, int_first_month, int_first_day, temp_start, int_first_month, int_first_day, temp_end, temp_type, temp_lecturer);
							if (strcmp(temp_frequency, "every") == 0)
								int_first_day += 7;
							else if (strcmp(temp_frequency, "throughout") == 0)
								int_first_day += 14;
							if (int_first_month == 1 || int_first_month == 3 || int_first_month == 5 || int_first_month == 7 || int_first_month == 8 || int_first_month == 10 || int_first_month == 12)
							{
								if (int_first_day > 31)
								{
									int_first_day -= 31;
									++int_first_month;
								}
							}
							else if (int_first_month == 4 || int_first_month == 6 || int_first_month == 9 || int_first_month == 11)
							{
								if (int_first_day > 30)
								{
									int_first_day -= 30;
									++int_first_month;
								}
							}
							else if (int_first_month == 2)
							{
								if (int_first_day > FEBRUARY)
								{
									int_first_day -= FEBRUARY;
									++int_first_month;
								}
							}
						} while (int_first_month != int_second_month || int_first_day != int_second_day);
						if (strlen(temp_classroom) == 0)
							fprintf_s(OutputFile, "\"%s%s\",\""YEAR".%02d.%02d\",\"%s\",\""YEAR".%02d.%02d\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", temp_title, temp_subgroup, int_first_month, int_first_day, temp_start, int_first_month, int_first_day, temp_end, temp_type, temp_lecturer);
						else
							fprintf_s(OutputFile, "\"%s. %s%s\",\""YEAR".%02d.%02d\",\"%s\",\""YEAR".%02d.%02d\",\"%s\",\"Ложь\",\"Ложь\",,,,,,,\"2\",\"Низкая\",,\"%s\",\"%s\",\"Обычный\",,,\"Ложь\"\n", temp_title, temp_classroom, temp_subgroup, int_first_month, int_first_day, temp_start, int_first_month, int_first_day, temp_end, temp_type, temp_lecturer);
					}
				}
			}
			fclose(SourceFile); // Закрытие потока с исходным файлом .json
			fclose(OutputFile); // Закрытие потока с выходным файлом .csv
			++number_of_saved_files;
		} while (_findnext(hJsonFile, &data) == 0);
	}
	_findclose(hJsonFile); // Закрытие знаковой целочисленной переменной hJsonFile
	
	endTime = getCPUTime();
	printf("Number of saved files = %d\n", number_of_saved_files);
	fprintf(stderr, "CPU time used = %.3lf msec\n", (endTime - startTime)*1000);
	system("pause");
	return 0;
}