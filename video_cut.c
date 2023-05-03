#include <iostream>
#include <string>
#include <sys/stat.h>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct frames {
    int index;
    char name[15];
    struct frames* next_frame;
};

struct clips {
    int index;
    char name[15];
    int first_frame;
    int last_frame;
    struct clips* next_clip;
};

struct cuts {
    int cut_frame;
    struct cuts* next_cut;
};

typedef struct frames* Frame_List;
typedef struct clips* Clip_List;
typedef struct cuts* Cut_List;

/********************Agregar nuevo frame a la lista*********************************/

void add_frame(Frame_List* list, struct frames* data) {
    if (*list == NULL) { // Si la lista está vacía, el nuevo nodo es el primero
        *list = data;
    }
    else { // Si la lista no está vacía, agregar el nuevo nodo al final
        struct frames* last_frame = *list;
        while (last_frame->next_frame != NULL) {
            last_frame = last_frame->next_frame;
        }
        last_frame->next_frame = data;
    }
}

/********************Agregar nuevo corte a la lista*********************************/

void add_cut(Cut_List* list, struct cuts* data) {
    if (*list == NULL) { // Si la lista está vacía, el nuevo nodo es el primero
        *list = data;
    }
    else { // Si la lista no está vacía, agregar el nuevo nodo al final
        struct cuts* last_cut = *list;
        while (last_cut->next_cut != NULL) {
            last_cut = last_cut->next_cut;
        }
        last_cut->next_cut = data;
    }
}

/********************Agregar nuevo clip a la lista*********************************/

void add_clip(Clip_List* list, struct clips* data) {
    if (*list == NULL) { // Si la lista está vacía, el nuevo nodo es el primero
        *list = data;
    }
    else { // Si la lista no está vacía, agregar el nuevo nodo al final
        struct clips* last_clip = *list;
        while (last_clip->next_clip != NULL) {
            last_clip = last_clip->next_clip;
        }
        last_clip->next_clip = data;
    }
}

/********************Ordenar lista de cortes*********************************/

void sort_cuts(Cut_List* list) {
    if (*list == NULL) {
        return; // La lista está vacía
    }

    int change = 1;
    while (change == 1) {
        change = 0;
        Cut_List ant = NULL;
        Cut_List act = *list;
        while (act->next_cut != NULL) {
            if (act->cut_frame > act->next_cut->cut_frame) {
                // Intercambiar los nodos
                Cut_List sig = act->next_cut;
                act->next_cut = sig->next_cut;
                sig->next_cut = act;
                if (ant == NULL) {
                    *list = sig;
                }
                else {
                    ant->next_cut = sig;
                }
                ant = sig;
                change = 1;
            }
            else {
                ant = act;
                act = act->next_cut;
            }
        }
    }
}

/********************Funcion de Bhattacharyya*********************************/

double bhattacharyya(double v1, double v2) {
    double res = 0;
    res = sqrt(v1 * v2);
    return res;
}

/********************Division de video en frames*********************************/

void splitVideoToFrames(char* videoPath, Frame_List* orig_frames_list, Frame_List* mod_frames_list) {

    // Creamos las carpetas para almacenar los frames originales y modificados
    std::string orig_frames_folder = "_original_frames";
    std::string mod_frames_folder = "_modified_frames";

    if (mkdir(orig_frames_folder.c_str(), 0777) == -1) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
    }
    if (mkdir(mod_frames_folder.c_str(), 0777) == -1) {
        std::cerr << "Error: " << strerror(errno) << std::endl;
    }

    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error al abrir el archivo de video." << std::endl;
        return;
    }

    int frame_index = 0;
    cv::Mat frame, hsv_frame;
    while (cap.read(frame)) {
        // Guardamos el frame original en la carpeta correspondiente
        std::string orig_frame_name = orig_frames_folder + "/" + std::to_string(frame_index) + ".jpg";
        cv::imwrite(orig_frame_name, frame);

        // Guardamos el frame modificado en la carpeta correspondiente

        cv::cvtColor(frame, hsv_frame, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> hsv_channels;
        cv::split(hsv_frame, hsv_channels);
        cv::normalize(hsv_channels[1], hsv_channels[1], 0, 255, cv::NORM_MINMAX);
        cv::normalize(hsv_channels[2], hsv_channels[2], 0, 255, cv::NORM_MINMAX);
        cv::merge(hsv_channels, hsv_frame);

        /* for (int i = 0; i < hsv_frame.rows; i++) {
             for (int j = 0; j < hsv_frame.cols; j++) {
                 hsv_frame.at<cv::Vec3b>(i, j)[0] = hsv_frame.at<cv::Vec3b>(i, j)[0] * 255 / 180;
                 hsv_frame.at<cv::Vec3b>(i, j)[1] = hsv_frame.at<cv::Vec3b>(i, j)[1] * 255 / 255;
                 hsv_frame.at<cv::Vec3b>(i, j)[2] = hsv_frame.at<cv::Vec3b>(i, j)[2] * 255 / 255;
             }
         }*/
        std::string mod_frame_name = mod_frames_folder + "/" + std::to_string(frame_index) + ".jpg";
        cv::imwrite(mod_frame_name, hsv_frame);

        // Agregamos los frames a las listas correspondientes
        struct frames* orig_frame = (struct frames*)malloc(sizeof(struct frames));
        orig_frame->index = frame_index;
        strcpy(orig_frame->name, orig_frame_name.c_str());
        orig_frame->next_frame = NULL;
        add_frame(orig_frames_list, orig_frame);

        struct frames* mod_frame = (struct frames*)malloc(sizeof(struct frames));
        mod_frame->index = frame_index;
        strcpy(mod_frame->name, mod_frame_name.c_str());
        mod_frame->next_frame = NULL;
        add_frame(mod_frames_list, mod_frame);

        frame_index++;
    }
    cap.release();
}

/******************Creacion del archivo txt de salida*************************/

void saveParams(int start, int end) {
    FILE* fp;
    fp = fopen("output/Output.txt", "a"); // Abrir el archivo en modo "a" para agregar información al final
    if (fp == NULL) {
        printf("Error al abrir el archivo");
        return;
    }
    fprintf(fp, "Se hizo un corte desde el frame: %d hasta el frame: %d\n", start, end); // Escribir los parámetros en una línea separados por un espacio y un salto de línea
    fclose(fp); // Cerrar el archivo
}

/******************Creacion de clips a partir de frames************************/

void createVideo(Clip_List temp_clip, int width, int height) {
    //int initFrame, int endFrame, int width, int height, char* framesFolder) {
    // Crear la carpeta de salida si no existe
    mkdir("output", 0777);

    // Crea el nombre del archivo de vídeo para la salida
    char outputVideoName[50];
    sprintf(outputVideoName, "output/%s", temp_clip->name);

    //Comienza a llenar el txt
    saveParams(temp_clip->first_frame, temp_clip->last_frame);

    // Abre el vídeo para comenzar a escribir en él
    cv::VideoWriter outputVideo;
    outputVideo.open(outputVideoName, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, cv::Size(width, height), true);

    // Recorre los frames y los añade al vídeo
    for (int i = temp_clip->first_frame; i <= temp_clip->last_frame; i++) {
        char frameName[50];
        sprintf(frameName, "_original_frames/%d.jpg", i);

        // Carga los frame y si es necesario les cambia el tamaño
        cv::Mat frame = cv::imread(frameName);
        if (frame.cols != width || frame.rows != height) {
            cv::resize(frame, frame, cv::Size(width, height));
        }


        // Añade los frame al vídeo
        outputVideo.write(frame);
    }

    // Libera memoria
    outputVideo.release();

    printf("Video guardado %s\n", outputVideoName);
}

int main(int argc, char** argv) //fatla agregar los args que recibe el programa archivo de video entrada, archivo de salida txt
{
    Frame_List normal_frame_list = NULL;
    Frame_List modify_frame_list = NULL;
    Frame_List temp_frame = NULL;

    Clip_List all_clips = NULL;
    Clip_List temp_clips = NULL;

    Cut_List all_cuts = NULL;
    Cut_List temp_cut = NULL;

    struct frames* new_frame;
    struct clips* new_clip;
    struct cuts* new_cut;

    double bch = 0, bcs = 0, bcv = 0;
    int first_frame = 0, index = 0, temp_index = 0;

    //Llama a la funcion para partir el video en frame agregar if rank=0 paralelizado
    splitVideoToFrames(argv[1], &normal_frame_list, &modify_frame_list);

    //Ejemplo de como irian los parametros del crear el video, esta función iria dentro de la función de cortar
    // createVideo(1, 100, 1280, 720, "_original_frames");

    temp_frame = modify_frame_list;

    while (temp_frame->next_frame != NULL) {
        bch = 0, bcs = 0, bcv = 0;
        double h1, h2, s1, s2, v1, v2;

        ////Agregar la carpeta
        char frameName1[50];
        char frameName2[50];
        sprintf(frameName1, "_original_frames/%d.jpg", temp_frame->name);
        sprintf(frameName2, "_original_frames/%d.jpg", temp_frame->next_frame->name);
        cv::Mat img1 = cv::imread(frameName1, cv::IMREAD_COLOR);
        cv::Mat img2 = cv::imread(frameName2, cv::IMREAD_COLOR);
        for (int i = 0; i < img1.cols; i++) {
            for (int j = 0; j < img1.rows; i++) {
                cv::Vec3b hsv_value1 = img1.at<cv::Vec3b>(i, j);
                cv::Vec3b hsv_value2 = img2.at<cv::Vec3b>(i, j);
                h1 = hsv_value1[0];
                s1 = hsv_value1[1];
                v1 = hsv_value1[2];
                h2 = hsv_value2[0];
                s2 = hsv_value2[1];
                v2 = hsv_value2[2];
                bch += bhattacharyya(h1, h2);
                bcs += bhattacharyya(s1, s2);
                bcv += bhattacharyya(v1, v2);
            }
        }
        if ((bch < 10) || (bcs < 10) || (bcv < 10)) {
            new_cut->cut_frame = temp_frame->index;
            new_cut->next_cut = NULL;
            add_cut(&all_cuts, new_cut);
        }
        temp_frame = temp_frame->next_frame;
    }

    sort_cuts(&all_cuts);

    temp_cut = all_cuts;
    index = 0;
    temp_index = 0;

    while (temp_cut !=NULL) {
        new_clip->first_frame = temp_index;
        new_clip->last_frame = temp_cut->cut_frame;
        new_clip->index = index;
        sprintf(new_clip->name, "Video_%d-%d.mp4", new_clip->first_frame, new_clip->last_frame);
        new_clip->next_clip = NULL;
        add_clip(&all_clips, new_clip);
        index++;
        temp_index = temp_cut->cut_frame;
        temp_cut = temp_cut->next_cut;
    }

    temp_clips = all_clips;

    char frameName[50];
    sprintf(frameName, "_original_frames/%d.jpg", normal_frame_list->name);
    cv::Mat img = cv::imread(frameName, cv::IMREAD_COLOR);

    while (temp_clips != NULL) {
        createVideo(temp_clips, img.cols, img.rows);
        temp_clips = temp_clips->next_clip;
    }
}
{
    Frame_List normal_frame_list = NULL;
    Frame_List modify_frame_list = NULL;
    Frame_List temp_frame = NULL;

    Clip_List all_clips = NULL;
    Clip_List temp_clips = NULL;

    Cut_List all_cuts = NULL;
    Cut_List temp_cut = NULL;

    struct frames* new_frame;
    struct clips* new_clip;
    struct cuts* new_cut;

    double bch = 0, bcs = 0, bcv = 0;
    int first_frame = 0, index = 0, temp_index = 0;

    //Llama a la funcion para partir el video en frame agregar if rank=0 paralelizado
    splitVideoToFrames(argv[1], &normal_frame_list, &modify_frame_list);

    temp_frame = modify_frame_list;

    while (temp_frame->next_frame != NULL) {
        bch = 0, bcs = 0, bcv = 0;
        double h1, h2, s1, s2, v1, v2;
        char frameName1[50];
        char frameName2[50];
        sprintf(frameName1, "_original_frames/%d.jpg", temp_frame->name);
        sprintf(frameName2, "_original_frames/%d.jpg", temp_frame->next_frame->name);
        cv::Mat img1 = cv::imread(frameName1, cv::IMREAD_COLOR);
        cv::Mat img2 = cv::imread(frameName2, cv::IMREAD_COLOR);
        for (int i = 0; i < img1.cols; i++) {
            for (int j = 0; j < img1.rows; i++) {
                cv::Vec3b hsv_value1 = img1.at<cv::Vec3b>(i, j);
                cv::Vec3b hsv_value2 = img2.at<cv::Vec3b>(i, j);
                h1 = hsv_value1[0];
                s1 = hsv_value1[1];
                v1 = hsv_value1[2];
                h2 = hsv_value2[0];
                s2 = hsv_value2[1];
                v2 = hsv_value2[2];
                bch += bhattacharyya(h1, h2);
                bcs += bhattacharyya(s1, s2);
                bcv += bhattacharyya(v1, v2);
            }
        }
        if ((bch < 10) || (bcs < 10) || (bcv < 10)) {
            new_cut->cut_frame = temp_frame->index;
            new_cut->next_cut = NULL;
            add_cut(&all_cuts, new_cut);
        }
        temp_frame = temp_frame->next_frame;
    }

    sort_cuts(&all_cuts);

    temp_cut = all_cuts;
    index = 0;
    temp_index = 0;

    while (temp_cut !=NULL) {
        new_clip->first_frame = temp_index;
        new_clip->last_frame = temp_cut->cut_frame;
        new_clip->index = index;
        sprintf(new_clip->name, "Video_%d-%d.mp4", new_clip->first_frame, new_clip->last_frame);
        new_clip->next_clip = NULL;
        add_clip(&all_clips, new_clip);
        index++;
        temp_index = temp_cut->cut_frame;
        temp_cut = temp_cut->next_cut;
    }

    temp_clips = all_clips;

    char frameName[50];
    sprintf(frameName, "_original_frames/%d.jpg", normal_frame_list->name);
    cv::Mat img = cv::imread(frameName, cv::IMREAD_COLOR);

    while (temp_clips != NULL) {
        createVideo(temp_clips, img.cols, img.rows);
        temp_clips = temp_clips->next_clip;
    }
}