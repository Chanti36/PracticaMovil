/*
 * INTRO SCENE
 * Copyright © 2020+ Santiago_Gil_Moya
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * santigil009@gmail.com
 */

#ifndef MENU_SCENE_HEADER
#define MENU_SCENE_HEADER

    #include <memory>
    #include <basics/Atlas>
    #include <basics/Canvas>
    #include <basics/Point>
    #include <basics/Scene>
    #include <basics/Size>
    #include <basics/Timer>

    namespace example
    {

        using basics::Atlas;
        using basics::Timer;
        using basics::Canvas;
        using basics::Point2f;
        using basics::Size2f;
        using basics::Texture_2D;
        using basics::Graphics_Context;

        class Menu_Scene : public basics::Scene
        {

            //Estados de la escena
            enum State
            {
                LOADING,
                READY,
                ERROR
            };

            enum Option_Id
            {
                PLAY,
                SCORES,
                HELP,
                CREDITS
            };

            //Plantilla para las opciones del mennu
            struct Option
            {
                const Atlas::Slice * slice;
                Point2f position;
                float   is_pressed;
            };

            static const unsigned number_of_options = 4;

        private:

            State    state;                                     //Estado de la escena.
            bool     suspended;                                 //Saber si la escena está en segundo plano

            unsigned canvas_width;                              //Ancho de la resolución virtual usada para dibujar.
            unsigned canvas_height;                             //Alto  de la resolución virtual usada para dibujar.

            Timer    timer;                                     //Cronómetro usado para medir intervalos de tiempo.

            Option   options[number_of_options];                //Datos de las opciones del menú

            std::unique_ptr< Atlas > atlas;                     //Atlas que contiene las imágenes de las opciones del menú

        public:

            Menu_Scene();

            //resolución virtual de la escena
            basics::Size2u get_view_size () override
            {
                return { canvas_width, canvas_height };
            }

            //inicializar atributos de la escena
            bool initialize () override;

            //Si el juego entra en segundo plano
            void suspend () override
            {
                suspended = true;
            }

            //Si el juego vuelve a primer plano
            void resume () override
            {
                suspended = false;
            }

            //Evento de interacción con la pantalla
            void handle (basics::Event & event) override;

            //Actualizar el estado cada frame
            void update (float time) override;

            //Dibujar los fotogramas
            void render (Graphics_Context::Accessor & context) override;

        private:

            //Establecer las propiedades de las opciones
            void configure_options ();

            //Devuelve la opción seleccionada
            int option_at (const Point2f & point);

        };

    }

#endif
