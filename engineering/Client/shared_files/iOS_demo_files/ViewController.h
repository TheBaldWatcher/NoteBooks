//
//  ViewController.h
//  Demo
//
//  Created by mango on 11/22/12.
//  Copyright (c) 2012 mango. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController {
    IBOutlet UIImageView *imageView;
}
@property (nonatomic, strong) UIImage *image;

- (void)prismaTest;
- (IBAction)oneClicked:(id)sender;
- (IBAction)twoClicked:(id)sender;

@end
