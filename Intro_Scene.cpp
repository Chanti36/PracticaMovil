/*
 * INTRO SCENE
 * Copyright © 2020+ Santiago_Gil_Moya
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * santigil009@gmail.com
 */

#include "Intro_Scene.hpp"
#include "Menu_Scene.hpp"
#include <basics/Canvas>
#include <basics/Director>

using namespace basics;
using namespace std;

namespace example
{

    // ---------------------------------------------------------------------------------------------

    bool Intro_Scene::initialize ()
    {
        if (state == UNINITIALIZED)
        {
            state = LOADING;
        }
        else
        {
            timer.reset ();

            opacity = 0.f;
            state   = FADING_IN;
        }
        return true;
    }

    // ---------------------------------------------------------------------------------------------

    void Intro_Scene::update (float time)
    {
        if (!suspended) switch (state)
        {
            case LOADING:    update_loading    (); break;
            case FADING_IN:  update_fading_in  (); break;
            case WAITING:    update_waiting    (); break;
            case FADING_OUT: update_fading_out (); break;
            default: break;
        }
    }

    // ---------------------------------------------------------------------------------------------

    void Intro_Scene::render (Graphics_Context::Accessor & context)
    {
        if (!suspended)
        {
            //Si no hay canvas crear uno

            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            if (!canvas)
            {
                 canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            // Ya con el canvas se puede dibujar en él

            if (canvas)
            {
                canvas->clear ();

                if (logo_texture)
                {
                    canvas->set_opacity (opacity);

                    canvas->fill_rectangle
                    (
                        { canvas_width * .5f, canvas_height * .5f },
                        { logo_texture->get_width (), logo_texture->get_height () },
                          logo_texture. get ()
                    );
                }
            }
        }
    }

    void Intro_Scene::update_loading ()
    {
        Graphics_Context::Accessor context = director.lock_graphics_context ();

        if (context)
        {
            // Cargar el logo comprobando que vaya bien

            logo_texture = Texture_2D::create (0, context, "logo.png");

            if (logo_texture)
            {
                context->add (logo_texture);

                timer.reset ();

                opacity = 0.f;
                state   = FADING_IN;
            }
            else
                state   = ERROR;
        }
    }

    void Intro_Scene::update_fading_in ()
    {
        float elapsed_seconds = timer.get_elapsed_seconds ();

        if (elapsed_seconds < 1.f)
        {
            opacity = elapsed_seconds;      //Animación fade in del logo
        }
        else
        {
            timer.reset ();

            opacity = 1.f;
            state   = WAITING;
        }
    }

    void Intro_Scene::update_waiting ()
    {
        // Esperar 2 segundos

        if (timer.get_elapsed_seconds () > 2.f)
        {
            timer.reset ();

            state = FADING_OUT;
        }
    }

    void Intro_Scene::update_fading_out ()
    {
        float elapsed_seconds = timer.get_elapsed_seconds ();

        if (elapsed_seconds < .5f)
        {
            opacity = 1.f - elapsed_seconds * 2.f;      //Animación fade out del logo
        }
        else
        {
            // Pasar a la siguiente escena una vez ha acabado la animación

            state = FINISHED;

            director.run_scene (shared_ptr< Scene >(new Menu_Scene));
        }
    }

}
