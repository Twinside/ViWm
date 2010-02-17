CC    := gcc
FLAGS := -Wall -Wextra -mwindows -s
OBJ	  := Layouter.o \
			LayoutTree.o \
			Renderer.o \
			Screen.o \
			State.o \
			TilledWindow.o \
			ViWm.o \
			ViWm.win32.o \
			ActionFunctors.o \
			Actions\TreeAction.o \
			Layouter\FullScreenLayout.o \
			Layouter\GridLayout.o \
			Layouter\HorizontalLayout.o \
			Layouter\ManualVimLayout.o \
			Layouter\VerticalLayout.o \
			main.o

viwm.exe: $(OBJ)
	#$(CC) ${FLAGS} -o $@ $<

%.o: %.cpp
	$(CC) ${FLAGS} -c $<

%.o: %.cc
	$(CC) ${FLAGS} -c $<

