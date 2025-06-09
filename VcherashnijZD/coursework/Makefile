# Makefile для компиляции LaTeX-документа

# Имя основного файла LaTeX
MAIN_FILE = report

# Файлы для компиляции
SRC_FILES = $(MAIN_FILE).tex

# Цель по умолчанию
all: pdf

# Правило для создания PDF
pdf: $(MAIN_FILE).pdf

# Правило для компиляции LaTeX-документа
$(MAIN_FILE).pdf: $(SRC_FILES)
	pdflatex -interaction=nonstopmode $(MAIN_FILE).tex & true
	pdflatex -interaction=nonstopmode $(MAIN_FILE).tex & true
	biber $(MAIN_FILE)
	pdflatex -interaction=nonstopmode $(MAIN_FILE).tex & true
	pdflatex -interaction=nonstopmode $(MAIN_FILE).tex & true

# Правило для очистки временных файлов
clean:
	rm -f *.aux *.log *.out *.toc *.bbl *.blg *.synctex.gz *.pdf

