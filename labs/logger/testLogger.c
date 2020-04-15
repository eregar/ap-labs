int panicf(const char *format, ...);
int errorf(const char *format, ...);
int warnf(const char *format, ...);
int infof(const char *format, ...);

int main() {
    infof("hello there! %d\n",2);
    errorf("this is %d error\n",1);
    warnf("this is %s\n","a warning");
    panicf("this is a panic\n");
    return 0;
}
