#ifndef MODEL_H
#define MODEL_H

struct model
{
    GLfloat *vertices;
    GLfloat *normals;
    GLuint *indices;

    size_t vsize;
    size_t nsize;
    size_t isize;
};

struct model * load_model(const char *path);
void model_free(struct model *m);

#endif /* MODEL_H */
