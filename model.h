#ifndef MODEL_H
#define MODEL_H

struct model
{
    GLfloat *coords;
    size_t csize;
    int nvertex;
};

struct model * load_model(const char *path);
void model_free(struct model *m);

#endif /* MODEL_H */
