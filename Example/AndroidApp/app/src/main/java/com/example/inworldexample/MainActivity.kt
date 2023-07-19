package com.example.inworldexample

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.example.inworldexample.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = InworldLoadScene()
    }

    /**
     * A native method that is implemented by the 'inworldexample' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String
    external fun InworldLoadScene(): String

    companion object {
        // Used to load the 'inworldexample' library on application startup.
        init {
            System.loadLibrary("inworldexample")
        }
    }
}