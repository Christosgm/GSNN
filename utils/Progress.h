#include <iostream>

class ProgressBar
{
    private:
        int iterations;
        int current_iteration = 0;
        std::string description; 
    public:
        ProgressBar(int iterations, std::string description)
        {
            this->iterations = iterations;
            this->description = description;
        }

        void update(int step)
        {
            current_iteration += step;
            show();
        }

        void show()
        {
            int percentage = (int)((float)current_iteration/iterations * 100);
            std::cout << "\r" << description << "(" << percentage << "%)[";
            for (int i = 0; i < percentage; i++) std::cout << ">";
            for (int i = 0; i < 100 - percentage; i++) std::cout << " ";
            std::cout << "]" << std::flush; 
        }
};