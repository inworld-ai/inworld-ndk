package com.example.inworldexample

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.CountDownTimer
import android.widget.TextView
import com.example.inworldexample.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        InworldStartClient()

        val time : Long = 10 * 60 * 1000 // 10 min
        object : CountDownTimer(time, 30) {
            override fun onTick(millisUntilFinished: Long) {
                InworldUpdateClient()
            }

            override fun onFinish() {
                InworldStopClient()
            }
        }.start()
    }

    /**
     * A native method that is implemented by the 'inworldexample' native library,
     * which is packaged with this application.
     */
    external fun InworldStartClient()
    external fun InworldUpdateClient()
    external fun InworldStopClient()

    companion object {
        // Used to load the 'inworldexample' library on application startup.
        init {
            System.loadLibrary("inworldexample")
        }
    }
}