#include <torch/extension.h>
#include <iostream>

int adder_forward(
	const torch::Tensor &input,
	const torch::Tensor &weight,
	// const torch::Tensor &bias,
	torch::Tensor &output,
	int KW, int KH,
	int SW, int SH,
	int PW, int PH
);

int adder_backward_grad_in(
	torch::Tensor &grad_out,
	torch::Tensor &input,
	torch::Tensor &weight,
	torch::Tensor &grad_in,
	int KW, int KH,
	int SW, int SH,
	int PW, int PH
);

int adder_backward_grad_weight(
	torch::Tensor &grad_out,
	torch::Tensor &input,
	torch::Tensor &weight,
	torch::Tensor &grad_weight,
	int KW, int KH,
	int SW, int SH,
	int PW, int PH
);

PYBIND11_MODULE(TORCH_EXTENSION_NAME, m)
{
    m.def("forward", &adder_forward, "adder forward");
    m.def("backward_input", &adder_backward_grad_in, "adder backward input");
    m.def("backward_weight", &adder_backward_grad_weight, "adder backward weight");
}

int adder_forward(
	const torch::Tensor &input,
	const torch::Tensor &weight,
	// const torch::Tensor &bias,
	torch::Tensor &output,
	int KW, int KH,
	int SW, int SH,
	int PW, int PH
) {

	int B = input.size(0);
	int CI = input.size(1);
	int CO = output.size(1);
	int IH = input.size(2);
	int IW = input.size(3);
	int OH = input.size(2);
	int OW = input.size(3);
	int image_offset0;
	int out_image_offset0;
	int weight_offset0;
	auto value = torch::zeros({1});
	
	for (int b = 0; b < B; b++)
	{
		for (int co = 0; co < CO; co++)
		{		
			out_image_offset0 = b*CO*IH*IW + co*IH*IW;
			for (int oh = 0; oh < OH; oh++)
			{		
				for (int ow = 0; ow < OW; ow++)
				{		
					for (int ci = 0; ci < CI; ci++)
					{		
						image_offset0 = b*CI*IH*IW + ci*IH*IW;
						weight_offset0 = co*CI*KH*KW + ci*KH*KW;


						value = torch::zeros({1});

						for (int kh = 0; kh < KH; kh++)
						{
							// #pragma unroll
							for (int kw = 0; kw < KW; kw++)
							{
								const int h = oh * SH - PH + kh;
								const int w = ow * SW - PW + kw;

								bool boundary_condition = (h >= 0) && (h < IH) && (w >= 0) && (w < IW);
								if (boundary_condition)
								{
									// value += input[image_offset0 + h * IW + w] * (*p_weight);
									value -= abs(input[image_offset0 + h * IW + w] - (weight[weight_offset0 + kh*KW + kh]));
								}
								else // padded area
								{
									value -= abs(weight[weight_offset0 + kh*KW + kh]);
								}
							}
						}
					}

					output[out_image_offset0 + oh*OW + ow] = value;
				}
			}
		}
	}

	return 1;
}

int adder_backward_grad_in(
	torch::Tensor &grad_out,
	torch::Tensor &input,
	torch::Tensor &weight,
	torch::Tensor &grad_in,
	int KW, int KH,
	int SW, int SH,
	int PW, int PH
) {
	/* To be implemented if needed, only forward implemented for FINN */
	grad_in = torch::zeros(
		{grad_in.size(0), grad_in.size(1), grad_in.size(2), grad_in.size(3)}
	);
	return 1;
}

int adder_backward_grad_weight(
	torch::Tensor &grad_out,
	torch::Tensor &input,
	torch::Tensor &weight,
	torch::Tensor &grad_weight,
	int KW, int KH,
	int SW, int SH,
	int PW, int PH
) {
	/* To be implemented if needed, only forward implemented for FINN */
	grad_weight = torch::zeros(
		{grad_weight.size(0), grad_weight.size(1), grad_weight.size(2), grad_weight.size(3)}
	);
	return 1;
}

