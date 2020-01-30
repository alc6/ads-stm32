/* Custom assert function */
/* Only use with DEBUG build */

#ifndef ADS_ASSERT_H_
#define ADS_ASSERT_H_


#define ASSERT(expr) \
    if (!(expr)) \
        aFailed(__FILE__, __LINE__)

void aFailed(char *file, int line);

#endif /* ADS_ASSERT_H_ */
