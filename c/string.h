#ifndef KN_STRING_H
#define KN_STRING_H

typedef struct kn_string_t kn_string_t;

kn_string_t *kn_string_intern(char const *);
kn_string_t *kn_string_alloced(char const *);
kn_string_t *kn_string_clone(kn_string_t const *);
char const *kn_string_deref(kn_string_t const *);
void kn_string_free(kn_string_t *);

#endif /* KN_STRING_H */
