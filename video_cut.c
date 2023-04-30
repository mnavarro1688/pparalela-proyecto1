#include <iostream>
#include <opencv2/opencv.hpp>

struct frames {
    int index;
    char name[15];
    struct frames* next_frame;
};

struct clips {
    int index;
    char name[15];
    struct clips* next_clip;
    struct frame* first_frame; 
};

struct cuts {
    int cut_frame;
    struct cuts* next_cut;
};

typedef struct frames* Frame_List;
typedef struct clips* Clip_List;
typedef struct cuts* Cut_List;

void add_frame(Frame_List* list, frames* data) {
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

void add_cut(Cut_List* list, cuts* data) {
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

void add_clip(Clip_List* list, clips* data) {
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

void sort_cuts(Cut_List* list) {
    //ordenar la lista de menor a mayor con respecto al cut_frame
}

float battachary(float v1, float v2) {
    //return op batacharrya
}

//----------------------Corta el video en frame y los guarda en una carpeta--------------------------------


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



int main(int argc, char** argv) //fatla agregar los args que recibe el programa archivo de video entrada, archivo de salida txt
{
    Frame_List normal_frame_list = NULL;
    Frame_List modify_frame_list = NULL;

    Clip_List all_clips = NULL;
    Cut_List aall_cuts = NULL;
    
    frames new_frame;
    clips new_clip;
    cuts new_cut;

    //Declarar aqui las variables restantes

  //Llama a la funcion para partir el video en frame
    splitVideoToFrames(argv[1], &normal_frame_list, &modify_frame_list);

    /*En esta seccion leemos cada frame y lo convertimos a hsv para obtener los valores a utilizar para calcualar batthacharya
        recordemos que battacharya es la sumatoria de 2 variables, por lo que aca trabajamos con una posicion de la lista y la siguiente    
        Luego de calculada el bataccharya de 2 frames continuos tenemos que agregar el condicional para determinar si es un frame de corte
            de ser frame de corte hay que agregarlo a la lista all_cuts donde:
                cut_frame: es el index del frame actual
                next_cut: seria null

                for(int i = 0 , i < width, i++)
                    for(int j = 0, j < heigth, j++)
                        valor hsv para img1, img2

                        img1: frame actual 
                        img2: frame siguiente

                sum_batt=sum_batt + batacharrya (im1, im2)

        En la version paralelizada agregar codigo para que todos los procesos envien su lista de cuts a un proceso X
        este los una y los ordene de menor a mayor con respecto a la variable  cut_frame y lo envie de vuelta a todos los procs
    */

    /*Continuamos con la reconstruccion de los clips, ya con la lista all_cuts ordenada se procede a la regeneracion de los clips
        mientras se genera el clip ir aregandolos a la lista all_clips
        (aca debemos idear un algoritmo para poder implementar paralelizacion en la regeneracion de clips)
            incluir una varible o un metodo para poder dividir la cantidad de cuts entre los procesos y cada proc
            vaya generando los clips

            contador cuts = 0
            Cut_list  total_cuts = all_cuts
            while(total_cuts != NULL){
                cuts++
                total_cuts = total_cuts -> next_cut
            }

            sort_clips(&all_clips) enviarlo a los demas procesos 

            part_size = cuts / size
            my_first = rank * part_size
            my_last = (rank + 1) * part_size

            if(rank = size-1)
                my_last = cuts


    */

    /*dar formato al archivo de salida imprimiendo lo que pide el proyecto
    * cantidad de clips y los frames que abarca....
    * El video tiene un total de ### clips divididos de la siguiente forma
    * 
    *   Clip #:
    *       
    */


}