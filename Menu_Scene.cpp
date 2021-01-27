/*
 * MENU SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Menu_Scene.hpp"
#include "Game_Scene.hpp"
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Transformation>

using namespace basics;
using namespace std;

namespace example
{

    Menu_Scene::Menu_Scene()
    {
        state         = LOADING;
        suspended     = true;
        canvas_width  =  720;
        canvas_height =  1280;

    }

    // ---------------------------------------------------------------------------------------------

    bool Menu_Scene::initialize ()
    {

        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::handle (basics::Event & event)
    {
        if (state == READY)                     // Se descartan los eventos cuando la escena está LOADING
        {
            switch (event.id)
            {
                case ID(touch-started):         // El usuario toca la pantalla
                case ID(touch-moved):
                {
                    // Se determina qué opción se ha tocado:

                    Point2f touch_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };


                    // Solo se puede tocar una opción a la vez (para evitar selecciones múltiples),
                    // por lo que solo una se considera presionada (el resto se "sueltan"):


                    break;
                }

                case ID(touch-ended):           // El usuario deja de tocar la pantalla
                {
                    Point2f touch_ended_location = { *event[ID(x)].as< var::Float > (), *event[ID(y)].as< var::Float > () };

                    x = *event[ID(x)].as< var::Float > ();
                    y = *event[ID(y)].as< var::Float > ();

                    director.run_scene (shared_ptr< Scene >(new Game_Scene));

                    break;
                }
            }
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::update (float time)
    {
        switch (state)
        {
            case LOADING: load();
                break;
            case READY: run();
                break;
        }

    }

    // ---------------------------------------------------------------------------------------------

    void Menu_Scene::render (Graphics_Context::Accessor & context)
    {
        if (!suspended && state == READY)
        {
            // El canvas se puede haber creado previamente, en cuyo caso solo hay que pedirlo:

            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            // Si no se ha creado previamente, hay que crearlo una vez:

            if (!canvas)
            {
                canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            // Si el canvas se ha podido obtener o crear, se puede dibujar con él:

            if (canvas)
            {
                canvas->clear ();

                canvas->fill_rectangle ({ canvas_width*.5f,canvas_height*.5f },                                     //FONDO
                                        { canvas_width*1.0f,canvas_height*1.0f },
                bg_texture.get());//CAMBIAR PARA QUE SE AJUSTE AL ALTO DE CADA PANTALLA EN FUNCIÓN DEL ANCHO


                canvas->fill_rectangle                                                               //TITLE
                        ({ canvas_width*.5f, canvas_height*.5f + (play_texture->get_height()*2.5f) },
                         {title_texture->get_width(),title_texture->get_height() },
                         title_texture.get());

                canvas->fill_rectangle                                                               //PLAY
                        ({ canvas_width*.5f, canvas_height*.5f + (play_texture->get_height()*.5f) },
                         {play_texture->get_width(),play_texture->get_height() },
                         play_texture.get());

                /*canvas->fill_rectangle                                                               //HELP NO FUNCIONA LA TEXTURA HELP
                        ({ canvas_width*.5f, canvas_height*.5f + (play_texture->get_height()*.5f) },
                         {help_texture->get_width(),help_texture->get_height() },
                         help_texture.get());*/

                canvas->fill_rectangle                                                               //CREDITS
                        ({ canvas_width*.5f, canvas_height*.5f - (play_texture->get_height()*1.5f) },
                         {credits_texture->get_width(),credits_texture->get_height() },
                         credits_texture.get());

                canvas->fill_rectangle                                                               //SCORE
                        ({ canvas_width*.5f, canvas_height*.5f - (play_texture->get_height()*2.5f) },
                         {score_texture->get_width(),score_texture->get_height() },
                         score_texture.get());


            }
        }
    }

    void Menu_Scene:: load()
    {
        if (!suspended)
        {
            Graphics_Context::Accessor context = director.lock_graphics_context ();

             if (context)
             {
                // Se cargan las texturas
                 bg_texture      = Texture_2D::create (ID(test), context, "menu-scene/menuscene.png");
                 title_texture   = Texture_2D::create (ID(test), context, "menu-scene/title.png");
                 credits_texture = Texture_2D::create (ID(test), context, "menu-scene/credits.png");
                 help_texture    = Texture_2D::create (ID(test), context, "menu-scene/help.png");
                 play_texture    = Texture_2D::create (ID(test), context, "menu-scene/play.png");
                 score_texture   = Texture_2D::create (ID(test), context, "menu-scene/score.png");

                context->add(bg_texture);
                context->add(credits_texture);
                context->add(title_texture);
                context->add(help_texture);
                context->add(play_texture);
                context->add(score_texture);

                state = READY;
             }
        }
    }

    void Menu_Scene:: run()
    {
        if(x < canvas_width*.5f+(play_texture->get_width()*.5f) && x > canvas_width+.5f-(play_texture->get_width()*.5f) )
        {
            if(y >canvas_height*.5f && y < canvas_height*.5f + play_texture->get_height())
            {
                director.run_scene (shared_ptr< Scene >(new Game_Scene));
            }
        }
    }

}