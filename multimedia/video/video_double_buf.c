#include "video_double_buf.h"
#include <stdlib.h>
#include <string.h>

VideoDoubleBuf* vdb_create(int size) {
    VideoDoubleBuf *v = calloc(1, sizeof(VideoDoubleBuf));
    if (!v) return NULL;
    v->buf[0] = malloc(size);
    v->buf[1] = malloc(size);
    if (!v->buf[0] || !v->buf[1]) {
        free(v->buf[0]); free(v->buf[1]); free(v);
        return NULL;
    }
    v->buf_size = size;
    v->write_idx = 0;
    v->ready_idx = -1;
    v->ui_busy = 0;
    v->stopped = 0;
    pthread_mutex_init(&v->lock, NULL);
    pthread_cond_init(&v->cond, NULL);
    return v;
}

void vdb_destroy(VideoDoubleBuf *vdb) {
    if (!vdb) return;
    
    // 标记停止，防止新的写入
    pthread_mutex_lock(&vdb->lock);
    vdb->stopped = 1;
    pthread_cond_signal(&vdb->cond);
    pthread_mutex_unlock(&vdb->lock);
    
    // 等待可能阻塞的操作完成
    usleep(10000);
    
    pthread_mutex_destroy(&vdb->lock);
    pthread_cond_destroy(&vdb->cond);
    free(vdb->buf[0]);
    free(vdb->buf[1]);
    free(vdb);
}

uint8_t* vdb_get_write_buf(VideoDoubleBuf *vdb) {
    pthread_mutex_lock(&vdb->lock);
    if (vdb->stopped) {
        pthread_mutex_unlock(&vdb->lock);
        return NULL;
    }
    uint8_t *ret = vdb->buf[vdb->write_idx];
    pthread_mutex_unlock(&vdb->lock);
    return ret;
}

void vdb_commit(VideoDoubleBuf *vdb) {
    pthread_mutex_lock(&vdb->lock);
    if (!vdb->stopped) {
        // 强制更新 ready_idx，即使 ui_busy
        // 这样可以确保最新的帧总是可用，避免画面卡住
        vdb->ready_idx = vdb->write_idx;
        vdb->write_idx ^= 1;
        vdb->ui_busy = 1;
        pthread_cond_signal(&vdb->cond);
    }
    pthread_mutex_unlock(&vdb->lock);
}

uint8_t* vdb_get_ready_buf(VideoDoubleBuf *vdb) {
    uint8_t *ret = NULL;
    pthread_mutex_lock(&vdb->lock);
    if (vdb->stopped) {
        pthread_mutex_unlock(&vdb->lock);
        return NULL;
    }
    if (vdb->ready_idx >= 0) {
        ret = vdb->buf[vdb->ready_idx];
    }
    pthread_mutex_unlock(&vdb->lock);
    return ret;
}

void vdb_release(VideoDoubleBuf *vdb) {
    pthread_mutex_lock(&vdb->lock);
    if (vdb->stopped) {
        pthread_mutex_unlock(&vdb->lock);
        return;
    }
    // 总是重置 ui_busy，确保解码器可以继续写入新帧
    vdb->ui_busy = 0;
    if (vdb->ready_idx >= 0) {
        vdb->ready_idx = -1;
    }
    pthread_mutex_unlock(&vdb->lock);
}