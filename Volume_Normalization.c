//Final Project
//By: Jean Luka Molina
//26/04/2021


    //Divide data by 4 for number of samples and divide by 2 for each channel
    //12552192 samples per channel 
    //5692 total block
    //divided by 2 we get 2846 blocks per channel
    //divide by Hz, you get 284.63s worth of data
    //divide by 60, you get 4.74 minutes

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <time.h>

typedef struct 
{
    char      Chunk_ID[4];
    long int  Chunk_size;
    char      Format[4];
    char      Subchunk_1_ID[4];
    long int  Subchunk_1_size;
    short int Audio_format;
    short int Num_channels;
    long int  Sample_rate;
    long int  Byte_rate;
    short int Block_align;
    short int Bits_per_sample;
    char      Dummy[44];
    char      Subchunk_2_ID[4];
    long int  Subchunk_2_size; 
} wav_format;

void printing_header(wav_format *header);
void average_volume(float *channel_1, float *channel_2, float *avg_1, float *avg_2, int samples_per_block, int blocks_per_channel);
float reference_volume(float *avg_1, float *avg_2, int blocks_per_channel);
void amplification_factor(float *amp_1, float *amp_2, float ref_volume, int blocks_per_channel);
void edit_samples(float *channel_1, float *channel_2, float *amp_1, float *amp_2, int samples_per_block, int blocks_per_channel);

void printing_header(wav_format *header)
{
    int count;
    float file_representation;
    for (count = 0; count < sizeof(header->Chunk_ID); count++)
    {
        if (count == 0)
        {
            printf("Chunk ID: ");
        }
        printf("%c", header->Chunk_ID[count]);
        if (count == 3)
        {
            printf("\n");
        }
    }
    
    file_representation = header->Chunk_size * (9.537 * pow(10, -7));
    printf("Chunk Size: %.2f MB / %lu Bytes \n", file_representation, header->Chunk_size);
    
    for (count = 0; count < sizeof(header->Format); count++)
    {
        if (count == 0)
        {
            printf("Format: ");
        }
        printf("%c", header->Format[count]);
        if (count == 3)
        {
            printf("\n");
        }
    }
    
    for (count = 0; count < sizeof(header->Subchunk_1_ID); count++)
    {
        if (count == 0)
        {
            printf("SubChunk 1: ");
        }
        printf("%c", header->Subchunk_1_ID[count]);
        if (count == 3)
        {
            printf("\n");
        }
    }
    
    printf("SubChunk 1 Size: %lu \n", header->Subchunk_1_size);
    printf("Audio Format: %hd \n", header->Audio_format);
    printf("Number of Channels: %hd \n", header->Num_channels);
    printf("Sample Rate: %lu \n", header->Sample_rate);
    printf("Byte Rate: %lu \n", header->Byte_rate);
    printf("Block Align: %hd \n", header->Block_align);
    printf("Bits Per Sample: %hd \n", header->Bits_per_sample);

    for (count = 0; count < sizeof(header->Subchunk_2_ID); count++)
    {
        if (count == 0)
        {
            printf("SubChunk 2: ");
        }
        printf("%c", header->Subchunk_2_ID[count]);
        if (count == 3)
        {
            printf("\n");
        }
    }
    
    printf("SubChunk 2 Size: %lu \n", header->Subchunk_2_size);

    
}

void average_volume(float *channel_1, float *channel_2, float *avg_1, float *avg_2, int samples_per_block, int blocks_per_channel)
{
    int count_1, count_2, index_1 = 0, index_2 = 0;
    float block_average_1, block_average_2;
    
    for (count_1 = 0; count_1 < blocks_per_channel; count_1++)
    {
        //Reset block averages to prevent accumulation
        block_average_1 = 0;
        block_average_2 = 0;
        for (count_2 = 0; count_2 < samples_per_block; count_2++)
        {
            //Add the absolute value of each sample to the current average
            block_average_1 += fabs(channel_1[index_2]);
            block_average_2 += fabs(channel_2[index_2]);
            index_2++;
        }
        
        //Compute the average of each block, both left and right channels
        block_average_1 = block_average_1 / (samples_per_block);
        block_average_2 = block_average_2 / (samples_per_block);
        
        //Put the value inside the arrays
        avg_1[index_1] = block_average_1;
        avg_2[index_1] = block_average_2;
        
        //increase the index for the averages of each channel
        index_1++;
    }
}

float reference_volume(float *avg_1, float *avg_2, int blocks_per_channel)
{

    int count;
    float max = avg_1[0];
    
    for (count = 0; count < blocks_per_channel; count++)
    {
        //Compare to the averages in the Left Channel
        if (avg_1[count] > max)
        {
            max = avg_1[count];
        }
        
        //Compare to the averages in the Right Channel
        if(avg_2[count] > max)
        {
            max = avg_2[count];
        }
    }

    return(max);
}

void amplification_factor(float *amp_1, float *amp_2, float ref_volume, int blocks_per_channel)
{
    int count;
    
    for (count = 0; count < blocks_per_channel; count++)
    {
        //Store in the array the amplification factor
        amp_1[count] = ref_volume / amp_1[count];
        //printf("Amplification factor for Left Block %d: %.5f \n", count, amp_1[count]);
        amp_2[count] = ref_volume / amp_2[count];
        //printf("Amplification factor for Right Block %d: %.5f \n", count, amp_2[count]);
    }
}

void edit_samples(float *channel_1, float *channel_2, float *amp_1, float *amp_2, int samples_per_block, int blocks_per_channel)
{
    int count_1, count_2, index_1 = 0, index_2 = 0;
    
    for (count_1 = 0; count_1 < blocks_per_channel; count_1++)
    {
        for (count_2 = 0; count_2 < samples_per_block; count_2++)
        {
            //Multiply the amplification factor to each sample
            channel_1[index_2] *= amp_1[index_1];
            channel_2[index_2] *= amp_2[index_1];
            index_2++;
        }
        index_1++;
    }
}

int main(void)
{
    clock_t initial_time, final_time;
    double time_required;
    FILE *infile;
    FILE *outfile;
    wav_format header;
    int count;
    int samples_per_channel;
    float *left_channel, *right_channel;
    float *left_temp, *right_temp;
    float *result_1, *result_2;
    int samples_per_block = 4410;
    int blocks_per_channel;
    float ref_volume;
    
    //Open the Original file
	infile = fopen("Kid A.wav", "rb");
    if (infile == NULL)
    {
        printf("Could not open file! \n");
        return (-1);
    }
    
    //Read the contents of the Header in the WAV file
    fread(&header, sizeof(wav_format), 1, infile);
    
    //Gather the samples_per_channel and allocate for each channel
    samples_per_channel = header.Subchunk_2_size / 2 / 4;
    left_channel = malloc(samples_per_channel * sizeof(float));
    right_channel = malloc(samples_per_channel * sizeof(float));
    
    //Allocate for the averages
    blocks_per_channel = samples_per_channel / samples_per_block;
    result_1 = malloc(blocks_per_channel * sizeof(float));
    result_2 = malloc(blocks_per_channel * sizeof(float));
    
    left_temp = left_channel;
    right_temp = right_channel;
    for (count = 0; count < samples_per_channel; count++)
    {
            fread(left_temp, sizeof(float),  1, infile);
            left_temp++;
            fread(right_temp, sizeof(float), 1, infile);
            right_temp++;
    }
    
    //Print the header
    printing_header(&header);
    
    //Compute the average volume and measure how long it takes
    initial_time = clock();
    average_volume(left_channel, right_channel, result_1, result_2, samples_per_block, blocks_per_channel);
    final_time = clock();
    time_required = (final_time - initial_time);
    printf("Time required to compute Average Volume: %.5lf Clock Units\n", time_required);
    
    //Find the max of a chosen block
    initial_time = clock();
    ref_volume = reference_volume(result_1, result_2, blocks_per_channel);
    final_time = clock();
    time_required = (final_time - initial_time);
    printf("Time required to get Reference Volume: %.5lf Clock Units\n", time_required);
    
    //Find the amplification factor
    initial_time = clock();
    amplification_factor(result_1, result_2, ref_volume, blocks_per_channel);
    final_time = clock();
    time_required = (final_time - initial_time);
    printf("Time required to find the Amplification Factor: %.5lf Clock Units\n", time_required);
    
    //Apply the amplification factor to each individual sample
    initial_time = clock();
    edit_samples(left_channel, right_channel, result_1, result_2, samples_per_block, blocks_per_channel);
    final_time = clock();
    time_required = (final_time - initial_time);
    printf("Time required to edit the Samples: %.5lf Clock Units\n", time_required);
    
    //Open the file that will be edited
    outfile = fopen("Edited Kid A.wav", "wb");
    if (outfile == NULL)
    {
        printf("Could not open file! \n");
        return (-1);
    }
    
    
    //Write the header and contents into the new file
    fwrite(&header, sizeof(wav_format), 1, outfile);
    
    left_temp = left_channel;
    right_temp = right_channel;
    for (count = 0; count < samples_per_channel; count++)
    {
            fwrite(left_temp, sizeof(float), 1, outfile);
            left_temp++;
            fwrite(right_temp, sizeof(float), 1, outfile);
            right_temp++;
    }

    //Close both files
    fclose(infile);
    fclose(outfile);
}
