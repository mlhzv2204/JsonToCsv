/*
 * Перекомпановка файла *.json в файл *.csv для работы с календарем
 * Специально для МГТУ СТАНКИН
 *
 */

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <errno.h>
#include <stdlib.h>
#include <windows.h>
#include <ctime> // Для clock()

#define CHAR 200
#define CHARTEMP 150 

int main()
{
	double start_time = clock();
	FILE* DbJsonFiles; // Файл с базой данных, который .json
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
	char DATE[] = "date\""; // Дата/даты проведения занятия/занятий
	//---------------------------------------------------------------------------------------
	int QuotationMarks = '"'; // Символ кавычки для работы с текстом файла
	errno_t err; // Возвращение ошибок при открытии файлов на ввод вывод

	SetConsoleOutputCP(65001);

	// Открытие файла для чтения данных с БД с наличием json файлов
	err = fopen_s(&DbJsonFiles, "C:\\CPrograms\\JSONToCSV\\DB with JSON Files\\DbJsonFiles.txt", "r");
	if (err == 0)
	{
		printf("Открыл файл DbJsonFiles.txt\n");
	}
	else
	{
		printf("Не могу открыть файл DbJsonFiles.txt\n");
		return 25;
	}

	while (fgets(NameOfJsonFile, CHAR, DbJsonFiles) != 0)
	{
		NameOfJsonFile[strlen(NameOfJsonFile) - 1] = '\0';

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
		strcpy_s(NameOfJsonFile, sizeof(NameOfJsonFile), strrchr(NameOfJsonFile, '\\') + 1);
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

		fprintf_s(OutputFile, "\"Тема\",\"Дата начала\",\"Время начала\",\"Дата завершения\",\"Время завершения\",\"Целый день\",\"Напоминание вкл / выкл\",\"Дата напоминания\",\"Время напоминания\",\"Организатор собрания\",\"Обязательные участники\",\"Необязательные участники\",\"Ресурсы собрания\",\"В это время\",\"Важность\",\"Категории\",\"Место\",\"Описание\",\"Пометка\",\"Расстояние\",\"Способ оплаты\",\"Частное\"\n");

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

			if (p = strstr(ReadyString, CLASSROOM))
			{
				p += strlen(CLASSROOM) + 4;
				correct = strchr(p, QuotationMarks);
				*correct = '\0';
				strcpy_s(TempCLASSROOM, sizeof(TempCLASSROOM), p);
			}

			if (p = strstr(ReadyString, START))
			{
				p += strlen(START) + 4;
				correct = strchr(p, QuotationMarks);
				*correct = '\0';
				strcpy_s(TempSTART, sizeof(TempSTART), p);
				strcat_s(TempSTART, sizeof(TempSTART), ":00");
			}

			if (p = strstr(ReadyString, END))
			{
				p += strlen(END) + 4;
				correct = strchr(p, QuotationMarks);
				*correct = '\0';
				strcpy_s(TempEND, sizeof(TempEND), p);
				strcat_s(TempEND, sizeof(TempEND), ":00");
			}

			if (p = strstr(ReadyString, FREQUENCY))
			{
				p += strlen(FREQUENCY) + 4;
				correct = strchr(p, QuotationMarks);
				*correct = '\0';
				strcpy_s(TempFREQUENCY, sizeof(TempFREQUENCY), p);
			}

			if (p = strstr(ReadyString, DATE))
			{
				p += strlen(DATE) + 3;
				correct = strchr(p, QuotationMarks);
				*correct = '\0';
				strcpy_s(TempDATE, sizeof(TempDATE), p);
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
		fclose(SourceFile);
		fclose(OutputFile);
	}
	fclose(DbJsonFiles);
	double end_time = clock();
	double search_time = end_time - start_time;
	printf("%f msec", search_time);
	_getch();
	return 0;
}