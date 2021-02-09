/*
 * MENU SCENE
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

#include "Sprite.hpp"


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

        typedef std::shared_ptr< basics::Texture_2D > Texture_Handle;


        //Estados de la escena
        enum State
        {
            LOADING,
            READY
        };



    private:



        State    state;                                     //Estado de la escena.
        bool     suspended;                                 //Determinar si la escena está en segundo plano

        unsigned canvas_width;                              // Ancho de la escena
        unsigned canvas_height;                             // Alto  de la escena

        Timer    timer;                                     // Cronómetro usado para medir intervalos de tiempo.

        Texture_Handle bg_texture,title_texture;            //Texturas del fondo y titulo

        Texture_Handle play_base,play_pressed,help_base,help_pressed,credits_base,credits_pressed, credits, help; // texturas de los botones

        bool onhelp,oncredits;                              //Determinar si está en submenus
        byte bottomover;                                    //Determinar el boton que pulsa
        float playpos,helppos,creditspos;                   //Determinar la posicion de los botones

        float x,y;


    public:

        Menu_Scene();

        //Conocer la resolucion
        basics::Size2u get_view_size () override
        {
            return { canvas_width, canvas_height };
        }

        //Inicializar valores
        bool initialize () override;

        //Cuando pasa a segundo plano
        void suspend () override
        {
            suspended = true;
        }

        //Cuando vuelve a primer plano
        void resume () override
        {
            suspended = false;
        }

        //Eventos de pantalla
        void handle (basics::Event & event) override;

        //Actualiza la escena
        void update (float time) override;

        //Dibuja la escena
        void render (Graphics_Context::Accessor & context) override;

    private:

        void load();
        void run();
    };

}

#endif
