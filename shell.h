struct cmd_s {
	char * cmd_name;
	funct_t* function;
	char * help;
};

void shell(void* arg);
void ps();
void ls();
void cat();
void compute();
void format_all();
