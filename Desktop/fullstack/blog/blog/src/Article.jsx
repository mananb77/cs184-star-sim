import './Article.css'
import React from 'react';

const Article = ({title, author, date, image, contents}) => {
    return (
        <>
            <div>
                <h1>{title}</h1>
                <p className='author'>{author} | {date}</p>
                <img className='articlePic' src={image}></img>
                <p className='content'>
                    {contents}
                </p>
            </div>

        </>
    );
};

export default Article;