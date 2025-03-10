# Content-Based Image Retrieval (CBIR) System

## Description
This application is designed to retrieve the top three most similar images to the selected image, and return the similarity index, Pearson correlation coefficient and the histograms.

## Purpose
This project enables users to select an image from a specific folder, and the system retrieves the top three most similar images based on visual features.

The user provides a value for M, which determines how the image will be divided into MxM grids. For each grid section, an RGB to HSI conversion is performed. 
The application then computes the average Hue for each area and uses this information, while building an array of average hues, to identify the most similar images using a formula that can be found in the documentation.

![image](https://github.com/user-attachments/assets/bbe8d681-d6ae-4060-af8e-44468eb8b9f0)

The project also contains the documentation: [`Documentatie.pdf`](Documentatie.pdf)


