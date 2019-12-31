#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

    void usleep(unsigned int usec);

#ifdef __cplusplus
}
#endif