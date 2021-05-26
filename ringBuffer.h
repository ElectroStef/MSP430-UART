typedef unsigned int rbd_t; //this descriptor will be used by the caller to access the ring buffer (apparently goes in header file)

typedef struct
{
    size_t s_elem; //the size of each element
    size_t n_elem; //the number of elements
    void *buffer;  //the pointer to the buffer that will hold the data
} rb_attr_t;


struct ringBuffer
{
    size_t s_elem;
    size_t n_elem;
    uint8_int *buf;
    volatile size_t head;
    volatile size_t tail;
};

static struct ringBuffer _rb[RING_BUFFER_MAX];

int ring_buffer_init(rbd_t *rbd, rb_attr_t *attr)
{
    static int idx = 0;
    int err = -1;

    if ((idx < RING_BUFFER_MAX) && (rbd != NULL) && (attr != NULL))
    {
        if((attr->buffer != NULL) && (attr->s_elem > 0))
        {
            if (((attr->n_elem - 1 ) & attr->n_elem) == 0)
            {
                _rb[idx].head = 0;
                _rb[idx].tail = 0;
                _rb[idx].buf = attr->buffer;
                _rb[idx].s_elem = attr->s_elem;
                _rb[idx].n_elem = attr->n_elem;

                *rbd = idx++;
                err = 0;
            }
        }
    }
    return err;
}