#include <iostream>
#include <sys/stat.h>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <unistd.h>
#include <time.h>

class clips {
public:
    int ini,fin;
    clips(int ini,int fin) : ini(ini), fin(fin){}
};

/***********************************Save text*********************************/

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

/***********************************Save speedup*********************************/

void txtSpeedup(double tserial, double tparalelo, int nproceso, char resulucion[]){
    FILE* fp2;
    double speedup= tserial/tparalelo;
    char guardar[100];

    fp2 = fopen("output/datos.txt", "a"); // Abrir el archivo en modo "a" para agregar información al final
    if (fp2 == NULL) {
        printf("Error al abrir el archivo");
        return;
    } 
    
    fprintf(fp2, "%f\t\t%d\t\t%s\n",speedup, nproceso, resulucion); // Escribir los parámetros en una línea separados por un espacio y un salto de línea
    fclose(fp2);
}


/***************************Creacion de clips**********************************/

void createVideo(int ini, int end, int width, int height) {
    // Crear la carpeta de salida si no existe
    mkdir("output", 0777);

    // Crea el nombre del archivo de vídeo para la salida
    char outputVideoName[50];
    sprintf(outputVideoName, "output/Video%d-%d.avi", ini,end);

    //Comienza a llenar el txt
    saveParams(ini, end);

    // Abre el vídeo para comenzar a escribir en él
    cv::VideoWriter outputVideo;
    outputVideo.open(outputVideoName, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 25, cv::Size(width, height), true);

    // Recorre los frames y los añade al vídeo
    for (int i = ini; i <= end; i++) {
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



int main(int argc, char** argv){

    clock_t inicio, fin;
    double tiempo;

    inicio = clock();

    std::list<int> original_frames;
    std::list<int> mod_frames;
    std::list<int> cuts;
    std::list<clips> clips_list;

    int h_bins = 200, s_bins = 200;
    int histSize[] = { h_bins, s_bins };
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    const float* ranges[] = { h_ranges, s_ranges };
    int channels[] = { 0, 1 };
    double btchr;

    clips new_clip = clips(0,0);

    char frameName1[50];
    char frameName2[50];

    int ini = 0;

    
    std::string orig_frames_folder = "_original_frames";
    std::string mod_frames_folder = "_modified_frames";

    mkdir(orig_frames_folder.c_str(), 0777);
    mkdir(mod_frames_folder.c_str(), 0777);

    cv::VideoCapture cap(argv[1]);
    if (!cap.isOpened()) {
        std::cerr << "Error al abrir el archivo de video." << std::endl;
    }

    int ancho = (int) cap.get(cv::CAP_PROP_FRAME_WIDTH); // obtener la anchura de los cuadros
    int alto = (int) cap.get(cv::CAP_PROP_FRAME_HEIGHT); // obtener la altura de los cuadros
    char resolucion[20];
    sprintf(resolucion, "%dx%d", ancho, alto);

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
        cv::normalize(hsv_channels[0], hsv_channels[0], 0, 255, cv::NORM_MINMAX);
        cv::normalize(hsv_channels[1], hsv_channels[1], 0, 255, cv::NORM_MINMAX);
        cv::normalize(hsv_channels[2], hsv_channels[2], 0, 255, cv::NORM_MINMAX);
        cv::merge(hsv_channels, hsv_frame);

        std::string mod_frame_name = mod_frames_folder + "/" + std::to_string(frame_index) + ".jpg";
        cv::imwrite(mod_frame_name, hsv_frame);

        // Agregamos los frames a las listas correspondientes
        original_frames.push_back(frame_index);
        mod_frames.push_back(frame_index);

        frame_index++;
    }
    cap.release();

    auto it = mod_frames.begin();
    while (std::distance(it, mod_frames.end()) >= 2) {
        int frame1 = *it;
        std::advance(it, 1);
        int frame2 = *it;
        sprintf(frameName1, "_modified_frames/%d.jpg", frame1);
        sprintf(frameName2, "_modified_frames/%d.jpg", frame2);
       
        cv::Mat hsv1 = cv::imread(frameName1, cv::IMREAD_COLOR);
        cv::Mat hsv2 = cv::imread(frameName2, cv::IMREAD_COLOR);
    
        cv::MatND hist1, hist2;
        cv::calcHist(&hsv1, 1, channels, cv::Mat(), hist1, 2, histSize, ranges, true, false);
        cv::calcHist(&hsv2, 1, channels, cv::Mat(), hist2, 2, histSize, ranges, true, false);

        cv::normalize(hist1, hist1, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
        cv::normalize(hist2, hist2, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

        btchr = cv::compareHist(hist1, hist2, cv::HISTCMP_BHATTACHARYYA);
        printf("bthcr : %f ---- %s \n", btchr, frameName2);
        if (btchr > 0.6) {
            cuts.push_back(frame1);    
        }
       
    }

    auto it2 = cuts.begin();
    while (it2 != cuts.end()) {
        int frame = *it2;
        clips_list.push_back(clips(ini, frame));
        ini = frame;
        ++it2;
    }

    if (ini < original_frames.back()){
        clips_list.push_back(clips(ini,original_frames.back()));
    }

    cv::Mat img = cv::imread("_original_frames/0.jpg", cv::IMREAD_COLOR);

    for (auto it = clips_list.begin(); it != clips_list.end(); ++it) {
        createVideo(it->ini, it->fin, img.cols, img.rows);
    }

    fin = clock();
    tiempo = ((double) (fin - inicio)) / CLOCKS_PER_SEC;
    //double tiempo serial, double tiempo paralelo, int numero de procesos, char resolucion
    txtSpeedup(tiempo, tiempo, 1, resolucion);
    
} 