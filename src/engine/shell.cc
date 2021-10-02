#include "shell.h"
#include "engine.h"

#include <mutex>
#include <readline/readline.h>
#include <readline/history.h>

std::mutex& shellLock() {
	static std::mutex m;
	return m;
}

int savedPoint;
char* savedText;

void saveReadline() {
	savedText = rl_copy_text(0, rl_end);
	savedPoint = rl_point;

	rl_save_prompt();
	rl_replace_line("", 0);
	rl_redisplay();
}

void loadReadline() {
	rl_restore_prompt();
	rl_replace_line(savedText, 0);
	rl_point = savedPoint;
	rl_redisplay();
	free(savedText);
}

int shellPrint(const char* format, ...) {
	lock_guard<mutex> lock(shellLock());
	
	saveReadline();

	va_list argptr;
	va_start(argptr, format);
	vprintf(format, argptr);
	va_end(argptr);

	loadReadline();
	
	return 0;
}

int shellPrintWarning(const char* format, ...) {
	lock_guard<mutex> lock(shellLock());
	
	saveReadline();

	va_list argptr;
	va_start(argptr, format);
	printWarning(format, argptr);
	va_end(argptr);

	loadReadline();
	
	return 0;
}

int shellPrintError(const char* format, ...) {
	lock_guard<mutex> lock(shellLock());
	
	saveReadline();

	va_list argptr;
	va_start(argptr, format);
	printError(format, argptr);
	va_end(argptr);

	loadReadline();
	
	return 0;
}

int vShellPrint(const char* format, va_list &args) {
	lock_guard<mutex> lock(shellLock());
	
	saveReadline();
	vprintf(format, args);
	loadReadline();
	
	return 0;
}

int vShellPrintWarning(const char* format, va_list &args) {
	lock_guard<mutex> lock(shellLock());
	
	saveReadline();
	printWarning(format, args);
	loadReadline();
	
	return 0;
}

int vShellPrintError(const char* format, va_list &args) {
	lock_guard<mutex> lock(shellLock());
	
	saveReadline();
	printError(format, args);
	loadReadline();
	
	return 0;
}

void Engine::enterShell() {
	this->printFunction = &shellPrint;
	this->warningFunction = &shellPrintWarning;
	this->errorFunction = &shellPrintError;
	
	this->interpreter->enterParallel();
	
	rl_bind_key('\t', rl_insert);

	char* shellBuffer;
	while((shellBuffer = readline("> ")) != nullptr) {
		if(strlen(shellBuffer) > 0) {
      add_history(shellBuffer);
    }

		rl_set_prompt("");
		rl_replace_line("", 0);
		rl_redisplay();

		string command(shellBuffer);
		this->execShell(command);

		free(shellBuffer);
  }
}
